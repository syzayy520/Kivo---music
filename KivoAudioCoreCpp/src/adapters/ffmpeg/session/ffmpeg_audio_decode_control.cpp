#include "adapters/ffmpeg/session/ffmpeg_audio_decode_runtime.hpp"

#include <limits>

extern "C" {
#include <libavutil/mathematics.h>
}

namespace kivo::adapters::ffmpeg::detail {

core::decode::DecodeControlResult FfmpegAudioDecodeRuntime::seek(
    core::contract::SamplePosition target_frame,
    core::decode::DecodeGeneration generation) noexcept {
    if (!opened_
        || !current_probe_.seekable
        || target_frame == core::contract::kInvalidSamplePosition
        || target_frame
            > static_cast<core::contract::SamplePosition>(
                std::numeric_limits<int64_t>::max())
        || (current_probe_.duration_known
            && target_frame >= current_probe_.duration_frames)) {
        return core::decode::DecodeControlResult::rejected(
            core::decode::DecodeFailure::InvalidRequest);
    }
    if (generation.id <= decode_generation_.id) {
        return core::decode::DecodeControlResult::rejected(
            core::decode::DecodeFailure::InvalidRequest);
    }
    const auto* stream = demux_.audio_stream();
    if (stream == nullptr || target_format_.format.sample_rate == 0) {
        return core::decode::DecodeControlResult::failed(
            core::decode::DecodeFailure::BoundaryFailure);
    }
    const auto timestamp = av_rescale_q(
        static_cast<int64_t>(target_frame),
        AVRational{
            1,
            static_cast<int>(target_format_.format.sample_rate)},
        stream->time_base);
    if (!demux_.seek_to(timestamp)) {
        return core::decode::DecodeControlResult::failed(demux_.failure());
    }
    reset_decode_state(generation);
    seek_target_frame_ = target_frame;
    next_frame_offset_ = target_frame;
    seek_pending_ = true;
    return core::decode::DecodeControlResult::succeeded();
}

core::decode::DecodeControlResult FfmpegAudioDecodeRuntime::flush(
    core::decode::DecodeGeneration generation) noexcept {
    if (!opened_ || generation.id <= decode_generation_.id) {
        return core::decode::DecodeControlResult::rejected(
            core::decode::DecodeFailure::InvalidRequest);
    }
    reset_decode_state(generation);
    return core::decode::DecodeControlResult::succeeded();
}

core::decode::DecodeControlResult FfmpegAudioDecodeRuntime::close() noexcept {
    converter_.close();
    decoder_.close();
    demux_.close();
    avio_.close();
    source_.reset();
    source_identity_ = {};
    source_generation_ = {};
    decode_generation_ = {};
    target_format_ = {};
    current_probe_ = {};
    next_frame_offset_ = 0;
    opened_ = false;
    input_eof_ = false;
    decoder_drain_sent_ = false;
    converter_drained_ = false;
    pending_converted_frame_ = false;
    seek_pending_ = false;
    seek_target_frame_ = 0;
    observed_leading_trim_ = 0;
    observed_trailing_trim_ = 0;
    return core::decode::DecodeControlResult::succeeded();
}

core::decode::MediaProbe FfmpegAudioDecodeRuntime::probe() const noexcept {
    return current_probe_;
}

void FfmpegAudioDecodeRuntime::reset_decode_state(
    core::decode::DecodeGeneration generation) noexcept {
    decoder_.flush();
    converter_.close();
    decode_generation_ = generation;
    current_probe_.decode_generation = generation;
    input_eof_ = false;
    decoder_drain_sent_ = false;
    converter_drained_ = false;
    pending_converted_frame_ = false;
    seek_pending_ = false;
}

} // namespace kivo::adapters::ffmpeg::detail
