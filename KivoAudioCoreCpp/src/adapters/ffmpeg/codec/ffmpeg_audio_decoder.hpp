#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
}

#include "kivo/core/decode/failure/decode_failure.hpp"

namespace kivo::adapters::ffmpeg::detail {

class FfmpegAudioDecoder {
public:
    FfmpegAudioDecoder() = default;
    ~FfmpegAudioDecoder();

    FfmpegAudioDecoder(const FfmpegAudioDecoder&) = delete;
    FfmpegAudioDecoder& operator=(const FfmpegAudioDecoder&) = delete;

    [[nodiscard]] bool open(const AVCodecParameters& parameters) noexcept;
    void close() noexcept;

    [[nodiscard]] int send_packet(const AVPacket* packet) noexcept;
    [[nodiscard]] int receive_frame() noexcept;
    void unref_packet() noexcept;
    void unref_frame() noexcept;
    void flush() noexcept;

    [[nodiscard]] AVPacket* packet() const noexcept;
    [[nodiscard]] AVFrame* frame() const noexcept;
    [[nodiscard]] AVCodecContext* context() const noexcept;
    [[nodiscard]] core::decode::DecodeFailure failure() const noexcept;

private:
    AVCodecContext* context_{nullptr};
    AVPacket* packet_{nullptr};
    AVFrame* frame_{nullptr};
    core::decode::DecodeFailure failure_{
        core::decode::DecodeFailure::None};
};

} // namespace kivo::adapters::ffmpeg::detail
