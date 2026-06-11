#pragma once

extern "C" {
#include <libavformat/avformat.h>
}

#include "kivo/core/decode/failure/decode_failure.hpp"

namespace kivo::adapters::ffmpeg::detail {

class FfmpegAvioBridge;

class FfmpegDemuxSession {
public:
    FfmpegDemuxSession() = default;
    ~FfmpegDemuxSession();

    FfmpegDemuxSession(const FfmpegDemuxSession&) = delete;
    FfmpegDemuxSession& operator=(const FfmpegDemuxSession&) = delete;

    [[nodiscard]] bool open(FfmpegAvioBridge& bridge) noexcept;
    void close() noexcept;

    [[nodiscard]] int read_audio_packet(AVPacket* packet) noexcept;
    [[nodiscard]] AVStream* audio_stream() const noexcept;
    [[nodiscard]] AVFormatContext* context() const noexcept;
    [[nodiscard]] int audio_stream_index() const noexcept;
    [[nodiscard]] core::decode::DecodeFailure failure() const noexcept;

private:
    AVFormatContext* context_{nullptr};
    int audio_stream_index_{-1};
    core::decode::DecodeFailure failure_{
        core::decode::DecodeFailure::None};
};

} // namespace kivo::adapters::ffmpeg::detail
