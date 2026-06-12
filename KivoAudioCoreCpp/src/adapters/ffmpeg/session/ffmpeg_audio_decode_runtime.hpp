#pragma once

#include <memory>

#include "kivo/adapters/ffmpeg/session/ffmpeg_audio_decode_session.hpp"
#include "adapters/ffmpeg/codec/ffmpeg_audio_decoder.hpp"
#include "adapters/ffmpeg/container/ffmpeg_demux_session.hpp"
#include "adapters/ffmpeg/conversion/converter/ffmpeg_audio_converter.hpp"
#include "adapters/ffmpeg/source/ffmpeg_avio_bridge.hpp"

namespace kivo::adapters::ffmpeg::detail {

class FfmpegAudioDecodeRuntime final {
public:
    [[nodiscard]] core::decode::DecodeOpenResult open(
        std::unique_ptr<core::decode::ByteSourceBoundary> new_source,
        const core::decode::DecodeOpenRequest& request) noexcept;
    [[nodiscard]] core::decode::DecodeStepResult decode_next() noexcept;
    [[nodiscard]] core::decode::DecodeControlResult seek(
        core::contract::SamplePosition target_frame,
        core::decode::DecodeGeneration generation) noexcept;
    [[nodiscard]] core::decode::DecodeControlResult flush(
        core::decode::DecodeGeneration generation) noexcept;
    core::decode::DecodeControlResult close() noexcept;
    [[nodiscard]] core::decode::MediaProbe probe() const noexcept;

private:
    [[nodiscard]] bool declared_duration_complete(
        core::contract::FrameCount additional_frames = 0) const noexcept;
    void reset_decode_state(
        core::decode::DecodeGeneration generation) noexcept;

    std::unique_ptr<core::decode::ByteSourceBoundary> source_{};
    FfmpegAvioBridge avio_{};
    FfmpegDemuxSession demux_{};
    FfmpegAudioDecoder decoder_{};
    FfmpegAudioConverter converter_{};
    core::contract::SourceIdentity source_identity_{};
    core::contract::SourceGeneration source_generation_{};
    core::decode::DecodeGeneration decode_generation_{};
    core::contract::RenderFormat target_format_{};
    core::processing::ResampleQuality resample_quality_{
        core::processing::ResampleQuality::Balanced};
    core::processing::DitherPolicy conversion_dither_{
        core::processing::DitherPolicy::Disabled};
    core::decode::MediaProbe current_probe_{};
    core::contract::SamplePosition next_frame_offset_{0};
    bool opened_{false};
    bool input_eof_{false};
    bool decoder_drain_sent_{false};
    bool converter_drained_{false};
    bool pending_converted_frame_{false};
    bool seek_pending_{false};
    core::contract::SamplePosition seek_target_frame_{0};
    uint32_t observed_leading_trim_{0};
    uint32_t observed_trailing_trim_{0};
};

} // namespace kivo::adapters::ffmpeg::detail
