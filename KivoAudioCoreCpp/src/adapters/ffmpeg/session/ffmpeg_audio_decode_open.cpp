#include "adapters/ffmpeg/session/ffmpeg_audio_decode_runtime.hpp"

#include <algorithm>
#include <utility>

extern "C" {
#include <libavutil/intreadwrite.h>
#include <libavutil/mathematics.h>
}

#include "adapters/ffmpeg/mapping/ffmpeg_audio_format.hpp"

namespace kivo::adapters::ffmpeg::detail {

namespace {

[[nodiscard]] core::contract::FrameCount duration_frames(
    const AVFormatContext& format,
    const AVStream& stream,
    uint32_t target_sample_rate,
    uint32_t source_sample_rate,
    uint64_t trim_samples) noexcept {
    int64_t duration = AV_NOPTS_VALUE;
    AVRational time_base{};
    if (stream.duration != AV_NOPTS_VALUE && stream.duration > 0) {
        duration = stream.duration;
        time_base = stream.time_base;
    } else if (format.duration != AV_NOPTS_VALUE && format.duration > 0) {
        duration = format.duration;
        time_base = AV_TIME_BASE_Q;
    }
    if (duration == AV_NOPTS_VALUE || target_sample_rate == 0) {
        return 0;
    }
    auto frames = av_rescale_q(
        duration,
        time_base,
        AVRational{1, static_cast<int>(target_sample_rate)});
    if (frames > 0 && trim_samples != 0 && source_sample_rate != 0) {
        const auto trimmed_frames = av_rescale_rnd(
            static_cast<int64_t>(trim_samples),
            target_sample_rate,
            source_sample_rate,
            AV_ROUND_UP);
        frames = std::max<int64_t>(frames - trimmed_frames, 0);
    }
    return frames > 0 ? static_cast<core::contract::FrameCount>(frames) : 0;
}

struct PacketTrim {
    uint32_t leading{0};
    uint32_t trailing{0};
};

[[nodiscard]] PacketTrim packet_trim(const AVPacket& packet) noexcept {
    size_t side_data_size = 0;
    const auto* side_data = av_packet_get_side_data(
        &packet,
        AV_PKT_DATA_SKIP_SAMPLES,
        &side_data_size);
    if (side_data == nullptr || side_data_size < 8) {
        return {};
    }
    return {
        AV_RL32(side_data),
        AV_RL32(side_data + 4)
    };
}

} // namespace

core::decode::DecodeOpenResult FfmpegAudioDecodeRuntime::open(
    std::unique_ptr<core::decode::ByteSourceBoundary> new_source,
    const core::decode::DecodeOpenRequest& request) noexcept {
    close();
    if (!new_source || !request.is_valid()) {
        return core::decode::DecodeOpenResult::rejected(
            core::decode::DecodeFailure::InvalidRequest);
    }

    source_identity_ = new_source->identity();
    source_generation_ = new_source->generation();
    decode_generation_ = request.decode_generation;
    target_format_ = request.target_format;
    source_ = std::move(new_source);

    if (source_identity_.value == 0 || !avio_.open(*source_)) {
        const auto failure = avio_.last_failure();
        close();
        return core::decode::DecodeOpenResult::failed(
            failure == core::decode::DecodeFailure::None
                ? core::decode::DecodeFailure::SourceOpenFailed
                : failure);
    }
    if (!demux_.open(avio_)) {
        const auto failure = demux_.failure();
        close();
        return core::decode::DecodeOpenResult::failed(failure);
    }
    auto* stream = demux_.audio_stream();
    if (stream == nullptr || stream->codecpar == nullptr) {
        close();
        return core::decode::DecodeOpenResult::failed(
            core::decode::DecodeFailure::NoAudioStream);
    }
    if (!decoder_.open(*stream->codecpar)) {
        const auto failure = decoder_.failure();
        close();
        return core::decode::DecodeOpenResult::failed(failure);
    }

    const auto codec = map_audio_codec(stream->codecpar->codec_id);
    const auto container = map_media_container(demux_.context()->iformat);
    if (codec == core::decode::AudioCodec::Unknown) {
        close();
        return core::decode::DecodeOpenResult::rejected(
            core::decode::DecodeFailure::UnsupportedCodec);
    }
    if (container == core::decode::MediaContainer::Unknown) {
        close();
        return core::decode::DecodeOpenResult::rejected(
            core::decode::DecodeFailure::UnsupportedContainer);
    }

    input_eof_ = false;
    decoder_drain_sent_ = false;
    converter_drained_ = false;
    for (;;) {
        const auto receive_result = decoder_.receive_frame();
        if (receive_result == 0) {
            break;
        }
        if (receive_result != AVERROR(EAGAIN)) {
            const auto failure = decoder_.failure();
            close();
            return core::decode::DecodeOpenResult::failed(
                receive_result == AVERROR_EOF
                    ? core::decode::DecodeFailure::InvalidMediaData
                    : failure);
        }
        decoder_.unref_packet();
        const auto read_result = demux_.read_audio_packet(decoder_.packet());
        if (read_result < 0) {
            const auto failure = demux_.failure();
            close();
            return core::decode::DecodeOpenResult::failed(
                read_result == AVERROR_EOF
                    ? core::decode::DecodeFailure::InvalidMediaData
                    : failure);
        }
        const auto trim = packet_trim(*decoder_.packet());
        observed_leading_trim_ =
            std::max(observed_leading_trim_, trim.leading);
        observed_trailing_trim_ =
            std::max(observed_trailing_trim_, trim.trailing);
        const auto send_result = decoder_.send_packet(decoder_.packet());
        decoder_.unref_packet();
        if (send_result < 0) {
            const auto failure = decoder_.failure();
            close();
            return core::decode::DecodeOpenResult::failed(failure);
        }
    }

    const auto native = map_native_format(
        static_cast<AVSampleFormat>(decoder_.frame()->format),
        decoder_.frame()->ch_layout,
        decoder_.frame()->sample_rate,
        stream->codecpar->bits_per_raw_sample);
    if (!native.has_value()) {
        close();
        return core::decode::DecodeOpenResult::rejected(
            core::decode::DecodeFailure::UnsupportedCodec);
    }

    const auto conversion = core::contract::ConversionPolicy{
        native->format.sample_format != target_format_.format.sample_format,
        native->format.sample_rate != target_format_.format.sample_rate,
        native->format.channel_layout != target_format_.format.channel_layout,
        native->format.bits_per_sample != target_format_.format.bits_per_sample
    };
    if (conversion.any_conversion_needed() && !request.allow_conversion) {
        close();
        return core::decode::DecodeOpenResult::rejected(
            core::decode::DecodeFailure::ConversionRequired);
    }
    if (!converter_.open(*decoder_.frame(), target_format_)
        || !converter_.convert(*decoder_.frame())) {
        const auto failure = converter_.failure();
        close();
        return core::decode::DecodeOpenResult::failed(failure);
    }
    pending_converted_frame_ = converter_.frame_count() != 0;

    current_probe_ = {};
    current_probe_.source_identity = source_identity_;
    current_probe_.source_generation = source_generation_;
    current_probe_.decode_generation = decode_generation_;
    current_probe_.codec = codec;
    current_probe_.container = container;
    current_probe_.native_format = *native;
    current_probe_.output_format = target_format_;
    current_probe_.conversion = conversion;
    current_probe_.resample = {
        conversion.sample_rate_convert,
        conversion.sample_rate_convert
            ? core::contract::ResampleReason::DeviceMismatch
            : core::contract::ResampleReason::NotNeeded,
        native->format.sample_rate,
        target_format_.format.sample_rate
    };
    current_probe_.codec_delay.samples = static_cast<uint64_t>(
        std::max(
            std::max(stream->codecpar->initial_padding, 0),
            static_cast<int>(observed_leading_trim_)));
    current_probe_.trailing_padding.samples = static_cast<uint64_t>(
        std::max(
            std::max(stream->codecpar->trailing_padding, 0),
            static_cast<int>(observed_trailing_trim_)));
    current_probe_.duration_frames = duration_frames(
        *demux_.context(),
        *stream,
        target_format_.format.sample_rate,
        native->format.sample_rate,
        current_probe_.codec_delay.samples
            + current_probe_.trailing_padding.samples);
    current_probe_.duration_known = current_probe_.duration_frames != 0;
    current_probe_.seekable = source_->seekable();
    if (!current_probe_.is_valid()) {
        close();
        return core::decode::DecodeOpenResult::failed(
            core::decode::DecodeFailure::BoundaryFailure);
    }

    opened_ = true;
    next_frame_offset_ = 0;
    return core::decode::DecodeOpenResult::accepted(current_probe_);
}

} // namespace kivo::adapters::ffmpeg::detail
