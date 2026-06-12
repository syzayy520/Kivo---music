#pragma once

#include <cstddef>
#include <span>
#include <vector>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
#include <libswresample/swresample.h>
}

#include "kivo/core/contract/format/roles/render_format.hpp"
#include "kivo/core/contract/sample_position.hpp"
#include "kivo/core/decode/failure/decode_failure.hpp"
#include "adapters/ffmpeg/mapping/ffmpeg_audio_format.hpp"

namespace kivo::adapters::ffmpeg::detail {

class FfmpegAudioConverter {
public:
    FfmpegAudioConverter() = default;
    ~FfmpegAudioConverter();

    FfmpegAudioConverter(const FfmpegAudioConverter&) = delete;
    FfmpegAudioConverter& operator=(const FfmpegAudioConverter&) = delete;

    [[nodiscard]] bool open(
        const AVFrame& frame,
        const core::contract::RenderFormat& target) noexcept;
    void close() noexcept;

    [[nodiscard]] bool is_open() const noexcept;
    [[nodiscard]] bool convert(const AVFrame& frame) noexcept;
    [[nodiscard]] bool drain() noexcept;

    [[nodiscard]] std::span<const std::byte> bytes() const noexcept;
    [[nodiscard]] core::contract::FrameCount frame_count() const noexcept;
    [[nodiscard]] core::decode::DecodeFailure failure() const noexcept;

private:
    [[nodiscard]] bool convert_samples(
        const uint8_t* const* input,
        int input_frames) noexcept;

    SwrContext* context_{nullptr};
    FfmpegTargetFormat target_{};
    core::contract::RenderFormat render_format_{};
    std::vector<std::byte> output_{};
    core::contract::FrameCount frame_count_{0};
    core::decode::DecodeFailure failure_{
        core::decode::DecodeFailure::None};
};

} // namespace kivo::adapters::ffmpeg::detail
