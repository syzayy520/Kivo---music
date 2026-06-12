#include "adapters/ffmpeg/session/ffmpeg_audio_decode_runtime.hpp"

#include <algorithm>
#include <limits>

extern "C" {
#include <libavutil/error.h>
#include <libavutil/mathematics.h>
}

namespace kivo::adapters::ffmpeg::detail {

core::decode::DecodeStepResult FfmpegAudioDecodeRuntime::decode_next() noexcept {
    if (!opened_) {
        return core::decode::DecodeStepResult::failed(
            core::decode::DecodeFailure::InvalidRequest);
    }

    if (pending_converted_frame_) {
        pending_converted_frame_ = false;
        const auto frame_count = converter_.frame_count();
        const core::decode::DecodedAudioBlockView block{
            converter_.bytes(),
            target_format_,
            frame_count,
            next_frame_offset_,
            source_generation_,
            decode_generation_,
            false
        };
        next_frame_offset_ += frame_count;
        return core::decode::DecodeStepResult::produced(block);
    }

    for (;;) {
        const auto receive_result = decoder_.receive_frame();
        if (receive_result == 0) {
            if (!converter_.is_open()
                && !converter_.open(*decoder_.frame(), target_format_)) {
                return core::decode::DecodeStepResult::failed(
                    converter_.failure());
            }
            if (!converter_.convert(*decoder_.frame())) {
                return core::decode::DecodeStepResult::failed(
                    converter_.failure());
            }
            auto frame_count = converter_.frame_count();
            auto bytes = converter_.bytes();
            if (frame_count == 0) {
                continue;
            }
            if (seek_pending_) {
                auto frame_start = seek_target_frame_;
                if (decoder_.frame()->best_effort_timestamp != AV_NOPTS_VALUE) {
                    const auto* stream = demux_.audio_stream();
                    if (stream != nullptr) {
                        const auto resolved = av_rescale_q(
                            decoder_.frame()->best_effort_timestamp,
                            stream->time_base,
                            AVRational{
                                1,
                                static_cast<int>(
                                    target_format_.format.sample_rate)});
                        if (resolved >= 0) {
                            frame_start =
                                static_cast<core::contract::SamplePosition>(
                                    resolved);
                        }
                    }
                }
                if (frame_start <= seek_target_frame_
                    && frame_count <= seek_target_frame_ - frame_start) {
                    continue;
                }
                const auto skip_frames = seek_target_frame_ > frame_start
                    ? seek_target_frame_ - frame_start
                    : 0;
                if (skip_frames >= frame_count) {
                    continue;
                }
                const auto skip_bytes = static_cast<size_t>(skip_frames)
                    * target_format_.format.bytes_per_frame();
                bytes = bytes.subspan(skip_bytes);
                frame_count -= skip_frames;
                next_frame_offset_ = std::max(
                    seek_target_frame_,
                    frame_start + skip_frames);
                seek_pending_ = false;
            }
            if (next_frame_offset_
                > std::numeric_limits<core::contract::SamplePosition>::max()
                    - frame_count) {
                return core::decode::DecodeStepResult::failed(
                    core::decode::DecodeFailure::BoundaryFailure);
            }
            const core::decode::DecodedAudioBlockView block{
                bytes,
                target_format_,
                frame_count,
                next_frame_offset_,
                source_generation_,
                decode_generation_,
                false
            };
            next_frame_offset_ += frame_count;
            return core::decode::DecodeStepResult::produced(block);
        }
        if (receive_result != AVERROR(EAGAIN)
            && receive_result != AVERROR_EOF) {
            return core::decode::DecodeStepResult::failed(decoder_.failure());
        }

        if (receive_result == AVERROR_EOF) {
            if (!converter_drained_) {
                if (!converter_.drain()) {
                    return core::decode::DecodeStepResult::failed(
                        converter_.failure());
                }
                converter_drained_ = converter_.frame_count() == 0;
                if (!converter_drained_) {
                    const auto frame_count = converter_.frame_count();
                    const core::decode::DecodedAudioBlockView block{
                        converter_.bytes(),
                        target_format_,
                        frame_count,
                        next_frame_offset_,
                        source_generation_,
                        decode_generation_,
                        true
                    };
                    next_frame_offset_ += frame_count;
                    return core::decode::DecodeStepResult::produced(block);
                }
            }
            return core::decode::DecodeStepResult::end_of_stream();
        }

        if (input_eof_) {
            if (!decoder_drain_sent_) {
                const auto send_result = decoder_.send_packet(nullptr);
                if (send_result < 0 && send_result != AVERROR_EOF) {
                    return core::decode::DecodeStepResult::failed(
                        decoder_.failure());
                }
                decoder_drain_sent_ = true;
                continue;
            }
            return core::decode::DecodeStepResult::failed(
                core::decode::DecodeFailure::InvalidMediaData);
        }

        decoder_.unref_packet();
        const auto read_result = demux_.read_audio_packet(decoder_.packet());
        if (read_result == AVERROR_EOF) {
            input_eof_ = true;
            continue;
        }
        if (read_result < 0) {
            return core::decode::DecodeStepResult::failed(demux_.failure());
        }
        const auto send_result = decoder_.send_packet(decoder_.packet());
        decoder_.unref_packet();
        if (send_result < 0 && send_result != AVERROR(EAGAIN)) {
            return core::decode::DecodeStepResult::failed(decoder_.failure());
        }
    }
}

} // namespace kivo::adapters::ffmpeg::detail
