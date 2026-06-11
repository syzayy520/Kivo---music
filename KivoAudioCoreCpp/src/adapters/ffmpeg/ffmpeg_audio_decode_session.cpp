#include "kivo/adapters/ffmpeg/ffmpeg_audio_decode_session.hpp"

#include <algorithm>
#include <limits>
#include <utility>

extern "C" {
#include <libavutil/error.h>
#include <libavutil/intreadwrite.h>
#include <libavutil/mathematics.h>
}

#include "codec/ffmpeg_audio_decoder.hpp"
#include "container/ffmpeg_demux_session.hpp"
#include "conversion/ffmpeg_audio_converter.hpp"
#include "mapping/ffmpeg_audio_format.hpp"
#include "source/ffmpeg_avio_bridge.hpp"

namespace kivo::adapters::ffmpeg {

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

class FfmpegAudioDecodeSession::Impl {
public:
    [[nodiscard]] core::decode::DecodeOpenResult open(
        std::unique_ptr<core::decode::ByteSourceBoundary> new_source,
        const core::decode::DecodeOpenRequest& request) noexcept {
        close();
        if (!new_source || !request.is_valid()) {
            return core::decode::DecodeOpenResult::rejected(
                core::decode::DecodeFailure::InvalidRequest);
        }

        source_identity = new_source->identity();
        source_generation = new_source->generation();
        decode_generation = request.decode_generation;
        target_format = request.target_format;
        source = std::move(new_source);

        if (source_identity.value == 0 || !avio.open(*source)) {
            const auto failure = avio.last_failure();
            close();
            return core::decode::DecodeOpenResult::failed(
                failure == core::decode::DecodeFailure::None
                    ? core::decode::DecodeFailure::SourceOpenFailed
                    : failure);
        }
        if (!demux.open(avio)) {
            const auto failure = demux.failure();
            close();
            return core::decode::DecodeOpenResult::failed(failure);
        }
        auto* stream = demux.audio_stream();
        if (stream == nullptr || stream->codecpar == nullptr) {
            close();
            return core::decode::DecodeOpenResult::failed(
                core::decode::DecodeFailure::NoAudioStream);
        }
        if (!decoder.open(*stream->codecpar)) {
            const auto failure = decoder.failure();
            close();
            return core::decode::DecodeOpenResult::failed(failure);
        }

        const auto codec = detail::map_audio_codec(stream->codecpar->codec_id);
        const auto container =
            detail::map_media_container(demux.context()->iformat);
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

        input_eof = false;
        decoder_drain_sent = false;
        converter_drained = false;
        for (;;) {
            const auto receive_result = decoder.receive_frame();
            if (receive_result == 0) {
                break;
            }
            if (receive_result != AVERROR(EAGAIN)) {
                const auto failure = decoder.failure();
                close();
                return core::decode::DecodeOpenResult::failed(
                    receive_result == AVERROR_EOF
                        ? core::decode::DecodeFailure::InvalidMediaData
                        : failure);
            }
            decoder.unref_packet();
            const auto read_result =
                demux.read_audio_packet(decoder.packet());
            if (read_result < 0) {
                const auto failure = demux.failure();
                close();
                return core::decode::DecodeOpenResult::failed(
                    read_result == AVERROR_EOF
                        ? core::decode::DecodeFailure::InvalidMediaData
                        : failure);
            }
            const auto trim = packet_trim(*decoder.packet());
            observed_leading_trim =
                std::max(observed_leading_trim, trim.leading);
            observed_trailing_trim =
                std::max(observed_trailing_trim, trim.trailing);
            const auto send_result =
                decoder.send_packet(decoder.packet());
            decoder.unref_packet();
            if (send_result < 0) {
                const auto failure = decoder.failure();
                close();
                return core::decode::DecodeOpenResult::failed(failure);
            }
        }

        const auto native = detail::map_native_format(
            static_cast<AVSampleFormat>(decoder.frame()->format),
            decoder.frame()->ch_layout,
            decoder.frame()->sample_rate,
            stream->codecpar->bits_per_raw_sample);
        if (!native.has_value()) {
            close();
            return core::decode::DecodeOpenResult::rejected(
                core::decode::DecodeFailure::UnsupportedCodec);
        }

        const auto conversion = core::contract::ConversionPolicy{
            native->format.sample_format
                != target_format.format.sample_format,
            native->format.sample_rate
                != target_format.format.sample_rate,
            native->format.channel_layout
                != target_format.format.channel_layout,
            native->format.bits_per_sample
                != target_format.format.bits_per_sample
        };
        if (conversion.any_conversion_needed() && !request.allow_conversion) {
            close();
            return core::decode::DecodeOpenResult::rejected(
                core::decode::DecodeFailure::ConversionRequired);
        }
        if (!converter.open(*decoder.frame(), target_format)
            || !converter.convert(*decoder.frame())) {
            const auto failure = converter.failure();
            close();
            return core::decode::DecodeOpenResult::failed(failure);
        }
        pending_converted_frame = converter.frame_count() != 0;

        current_probe = {};
        current_probe.source_identity = source_identity;
        current_probe.source_generation = source_generation;
        current_probe.decode_generation = decode_generation;
        current_probe.codec = codec;
        current_probe.container = container;
        current_probe.native_format = *native;
        current_probe.output_format = target_format;
        current_probe.conversion = conversion;
        current_probe.resample = {
            conversion.sample_rate_convert,
            conversion.sample_rate_convert
                ? core::contract::ResampleReason::DeviceMismatch
                : core::contract::ResampleReason::NotNeeded,
            native->format.sample_rate,
            target_format.format.sample_rate
        };
        current_probe.codec_delay.samples = static_cast<uint64_t>(
            std::max(
                std::max(stream->codecpar->initial_padding, 0),
                static_cast<int>(observed_leading_trim)));
        current_probe.trailing_padding.samples = static_cast<uint64_t>(
            std::max(
                std::max(stream->codecpar->trailing_padding, 0),
                static_cast<int>(observed_trailing_trim)));
        current_probe.duration_frames = duration_frames(
            *demux.context(),
            *stream,
            target_format.format.sample_rate,
            native->format.sample_rate,
            current_probe.codec_delay.samples
                + current_probe.trailing_padding.samples);
        current_probe.duration_known = current_probe.duration_frames != 0;
        current_probe.seekable = source->seekable();
        if (!current_probe.is_valid()) {
            close();
            return core::decode::DecodeOpenResult::failed(
                core::decode::DecodeFailure::BoundaryFailure);
        }

        opened = true;
        next_frame_offset = 0;
        return core::decode::DecodeOpenResult::accepted(current_probe);
    }

    [[nodiscard]] core::decode::DecodeStepResult decode_next() noexcept {
        if (!opened) {
            return core::decode::DecodeStepResult::failed(
                core::decode::DecodeFailure::InvalidRequest);
        }

        if (pending_converted_frame) {
            pending_converted_frame = false;
            const auto frame_count = converter.frame_count();
            const core::decode::DecodedAudioBlockView block{
                converter.bytes(),
                target_format,
                frame_count,
                next_frame_offset,
                source_generation,
                decode_generation,
                false
            };
            next_frame_offset += frame_count;
            return core::decode::DecodeStepResult::produced(block);
        }

        for (;;) {
            const auto receive_result = decoder.receive_frame();
            if (receive_result == 0) {
                if (!converter.is_open()
                    && !converter.open(*decoder.frame(), target_format)) {
                    return core::decode::DecodeStepResult::failed(
                        converter.failure());
                }
                if (!converter.convert(*decoder.frame())) {
                    return core::decode::DecodeStepResult::failed(
                        converter.failure());
                }
                auto frame_count = converter.frame_count();
                auto bytes = converter.bytes();
                if (frame_count == 0) {
                    continue;
                }
                if (seek_pending) {
                    auto frame_start = seek_target_frame;
                    if (decoder.frame()->best_effort_timestamp != AV_NOPTS_VALUE) {
                        const auto* stream = demux.audio_stream();
                        if (stream != nullptr) {
                            const auto resolved = av_rescale_q(
                                decoder.frame()->best_effort_timestamp,
                                stream->time_base,
                                AVRational{
                                    1,
                                    static_cast<int>(
                                        target_format.format.sample_rate)});
                            if (resolved >= 0) {
                                frame_start = static_cast<
                                    core::contract::SamplePosition>(resolved);
                            }
                        }
                    }
                    if (frame_start <= seek_target_frame
                        && frame_count
                            <= seek_target_frame - frame_start) {
                        continue;
                    }
                    const auto skip_frames = seek_target_frame > frame_start
                        ? seek_target_frame - frame_start
                        : 0;
                    if (skip_frames >= frame_count) {
                        continue;
                    }
                    const auto skip_bytes = static_cast<size_t>(skip_frames)
                        * target_format.format.bytes_per_frame();
                    bytes = bytes.subspan(skip_bytes);
                    frame_count -= skip_frames;
                    next_frame_offset = std::max(
                        seek_target_frame,
                        frame_start + skip_frames);
                    seek_pending = false;
                }
                if (next_frame_offset
                    > std::numeric_limits<core::contract::SamplePosition>::max()
                        - frame_count) {
                    return core::decode::DecodeStepResult::failed(
                        core::decode::DecodeFailure::BoundaryFailure);
                }
                const core::decode::DecodedAudioBlockView block{
                    bytes,
                    target_format,
                    frame_count,
                    next_frame_offset,
                    source_generation,
                    decode_generation,
                    false
                };
                next_frame_offset += frame_count;
                return core::decode::DecodeStepResult::produced(block);
            }
            if (receive_result != AVERROR(EAGAIN)
                && receive_result != AVERROR_EOF) {
                return core::decode::DecodeStepResult::failed(
                    decoder.failure());
            }

            if (receive_result == AVERROR_EOF) {
                if (!converter_drained) {
                    if (!converter.drain()) {
                        return core::decode::DecodeStepResult::failed(
                            converter.failure());
                    }
                    converter_drained = converter.frame_count() == 0;
                    if (!converter_drained) {
                        const auto frame_count = converter.frame_count();
                        const core::decode::DecodedAudioBlockView block{
                            converter.bytes(),
                            target_format,
                            frame_count,
                            next_frame_offset,
                            source_generation,
                            decode_generation,
                            true
                        };
                        next_frame_offset += frame_count;
                        return core::decode::DecodeStepResult::produced(block);
                    }
                }
                return core::decode::DecodeStepResult::end_of_stream();
            }

            if (input_eof) {
                if (!decoder_drain_sent) {
                    const auto send_result = decoder.send_packet(nullptr);
                    if (send_result < 0 && send_result != AVERROR_EOF) {
                        return core::decode::DecodeStepResult::failed(
                            decoder.failure());
                    }
                    decoder_drain_sent = true;
                    continue;
                }
                return core::decode::DecodeStepResult::failed(
                    core::decode::DecodeFailure::InvalidMediaData);
            }

            decoder.unref_packet();
            const auto read_result =
                demux.read_audio_packet(decoder.packet());
            if (read_result == AVERROR_EOF) {
                input_eof = true;
                continue;
            }
            if (read_result < 0) {
                return core::decode::DecodeStepResult::failed(
                    demux.failure());
            }
            const auto send_result =
                decoder.send_packet(decoder.packet());
            decoder.unref_packet();
            if (send_result < 0 && send_result != AVERROR(EAGAIN)) {
                return core::decode::DecodeStepResult::failed(
                    decoder.failure());
            }
        }
    }

    [[nodiscard]] core::decode::DecodeControlResult seek(
        core::contract::SamplePosition target_frame,
        core::decode::DecodeGeneration generation) noexcept {
        if (!opened
            || !current_probe.seekable
            || target_frame == core::contract::kInvalidSamplePosition
            || target_frame
                > static_cast<core::contract::SamplePosition>(
                    std::numeric_limits<int64_t>::max())
            || (current_probe.duration_known
                && target_frame >= current_probe.duration_frames)) {
            return core::decode::DecodeControlResult::rejected(
                core::decode::DecodeFailure::InvalidRequest);
        }
        if (generation.id <= decode_generation.id) {
            return core::decode::DecodeControlResult::rejected(
                core::decode::DecodeFailure::InvalidRequest);
        }
        const auto* stream = demux.audio_stream();
        if (stream == nullptr || target_format.format.sample_rate == 0) {
            return core::decode::DecodeControlResult::failed(
                core::decode::DecodeFailure::BoundaryFailure);
        }
        const auto timestamp = av_rescale_q(
            static_cast<int64_t>(target_frame),
            AVRational{
                1,
                static_cast<int>(target_format.format.sample_rate)},
            stream->time_base);
        if (!demux.seek_to(timestamp)) {
            return core::decode::DecodeControlResult::failed(
                demux.failure());
        }
        reset_decode_state(generation);
        seek_target_frame = target_frame;
        next_frame_offset = target_frame;
        seek_pending = true;
        return core::decode::DecodeControlResult::succeeded();
    }

    [[nodiscard]] core::decode::DecodeControlResult flush(
        core::decode::DecodeGeneration generation) noexcept {
        if (!opened) {
            return core::decode::DecodeControlResult::rejected(
                core::decode::DecodeFailure::InvalidRequest);
        }
        if (generation.id <= decode_generation.id) {
            return core::decode::DecodeControlResult::rejected(
                core::decode::DecodeFailure::InvalidRequest);
        }
        reset_decode_state(generation);
        return core::decode::DecodeControlResult::succeeded();
    }

    core::decode::DecodeControlResult close() noexcept {
        converter.close();
        decoder.close();
        demux.close();
        avio.close();
        source.reset();
        source_identity = {};
        source_generation = {};
        decode_generation = {};
        target_format = {};
        current_probe = {};
        next_frame_offset = 0;
        opened = false;
        input_eof = false;
        decoder_drain_sent = false;
        converter_drained = false;
        pending_converted_frame = false;
        seek_pending = false;
        seek_target_frame = 0;
        observed_leading_trim = 0;
        observed_trailing_trim = 0;
        return core::decode::DecodeControlResult::succeeded();
    }

    void reset_decode_state(
        core::decode::DecodeGeneration generation) noexcept {
        decoder.flush();
        converter.close();
        decode_generation = generation;
        current_probe.decode_generation = generation;
        input_eof = false;
        decoder_drain_sent = false;
        converter_drained = false;
        pending_converted_frame = false;
        seek_pending = false;
    }

    std::unique_ptr<core::decode::ByteSourceBoundary> source{};
    detail::FfmpegAvioBridge avio{};
    detail::FfmpegDemuxSession demux{};
    detail::FfmpegAudioDecoder decoder{};
    detail::FfmpegAudioConverter converter{};
    core::contract::SourceIdentity source_identity{};
    core::contract::SourceGeneration source_generation{};
    core::decode::DecodeGeneration decode_generation{};
    core::contract::RenderFormat target_format{};
    core::decode::MediaProbe current_probe{};
    core::contract::SamplePosition next_frame_offset{0};
    bool opened{false};
    bool input_eof{false};
    bool decoder_drain_sent{false};
    bool converter_drained{false};
    bool pending_converted_frame{false};
    bool seek_pending{false};
    core::contract::SamplePosition seek_target_frame{0};
    uint32_t observed_leading_trim{0};
    uint32_t observed_trailing_trim{0};
};

FfmpegAudioDecodeSession::FfmpegAudioDecodeSession()
    : impl_(std::make_unique<Impl>()) {}

FfmpegAudioDecodeSession::~FfmpegAudioDecodeSession() = default;

core::decode::DecodeOpenResult FfmpegAudioDecodeSession::open(
    std::unique_ptr<core::decode::ByteSourceBoundary> source,
    const core::decode::DecodeOpenRequest& request) noexcept {
    if (!impl_) {
        return core::decode::DecodeOpenResult::failed(
            core::decode::DecodeFailure::BoundaryFailure);
    }
    try {
        return impl_->open(std::move(source), request);
    } catch (...) {
        impl_->close();
        return core::decode::DecodeOpenResult::failed(
            core::decode::DecodeFailure::BoundaryFailure);
    }
}

core::decode::DecodeStepResult
FfmpegAudioDecodeSession::decode_next() noexcept {
    if (!impl_) {
        return core::decode::DecodeStepResult::failed(
            core::decode::DecodeFailure::BoundaryFailure);
    }
    try {
        return impl_->decode_next();
    } catch (...) {
        return core::decode::DecodeStepResult::failed(
            core::decode::DecodeFailure::BoundaryFailure);
    }
}

core::decode::DecodeControlResult FfmpegAudioDecodeSession::seek(
    core::contract::SamplePosition target_frame,
    core::decode::DecodeGeneration generation) noexcept {
    if (!impl_) {
        return core::decode::DecodeControlResult::failed(
            core::decode::DecodeFailure::BoundaryFailure);
    }
    try {
        return impl_->seek(target_frame, generation);
    } catch (...) {
        return core::decode::DecodeControlResult::failed(
            core::decode::DecodeFailure::BoundaryFailure);
    }
}

core::decode::DecodeControlResult FfmpegAudioDecodeSession::flush(
    core::decode::DecodeGeneration generation) noexcept {
    if (!impl_) {
        return core::decode::DecodeControlResult::failed(
            core::decode::DecodeFailure::BoundaryFailure);
    }
    try {
        return impl_->flush(generation);
    } catch (...) {
        return core::decode::DecodeControlResult::failed(
            core::decode::DecodeFailure::BoundaryFailure);
    }
}

core::decode::DecodeControlResult FfmpegAudioDecodeSession::close() noexcept {
    return impl_
        ? impl_->close()
        : core::decode::DecodeControlResult::no_op();
}

core::decode::MediaProbe FfmpegAudioDecodeSession::probe() const noexcept {
    return impl_ ? impl_->current_probe : core::decode::MediaProbe{};
}

} // namespace kivo::adapters::ffmpeg
