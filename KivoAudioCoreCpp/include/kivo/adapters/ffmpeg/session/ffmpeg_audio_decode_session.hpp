#pragma once

#include <memory>

#include "kivo/core/decode/boundary/audio_decode_boundary.hpp"

namespace kivo::adapters::ffmpeg {

namespace detail {
class FfmpegAudioDecodeRuntime;
}

class FfmpegAudioDecodeSession final
    : public core::decode::AudioDecodeBoundary {
public:
    FfmpegAudioDecodeSession();
    ~FfmpegAudioDecodeSession() override;

    FfmpegAudioDecodeSession(const FfmpegAudioDecodeSession&) = delete;
    FfmpegAudioDecodeSession& operator=(const FfmpegAudioDecodeSession&) = delete;
    FfmpegAudioDecodeSession(FfmpegAudioDecodeSession&&) = delete;
    FfmpegAudioDecodeSession& operator=(FfmpegAudioDecodeSession&&) = delete;

    [[nodiscard]] core::decode::DecodeOpenResult open(
        std::unique_ptr<core::decode::ByteSourceBoundary> source,
        const core::decode::DecodeOpenRequest& request) noexcept override;
    [[nodiscard]] core::decode::DecodeStepResult decode_next() noexcept override;
    [[nodiscard]] core::decode::DecodeControlResult seek(
        core::contract::SamplePosition target_frame,
        core::decode::DecodeGeneration generation) noexcept override;
    [[nodiscard]] core::decode::DecodeControlResult flush(
        core::decode::DecodeGeneration generation) noexcept override;
    [[nodiscard]] core::decode::DecodeControlResult close() noexcept override;
    [[nodiscard]] core::decode::MediaProbe probe() const noexcept override;

private:
    std::unique_ptr<detail::FfmpegAudioDecodeRuntime> impl_;
};

} // namespace kivo::adapters::ffmpeg
