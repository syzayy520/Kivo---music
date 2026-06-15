#pragma once

#include <cstdint>

extern "C" {
#include <libavformat/avio.h>
}

#include "kivo/core/decode/source/byte_source_boundary.hpp"

namespace kivo::adapters::ffmpeg::detail {

class FfmpegAvioBridge {
public:
    FfmpegAvioBridge() = default;
    ~FfmpegAvioBridge();

    FfmpegAvioBridge(const FfmpegAvioBridge&) = delete;
    FfmpegAvioBridge& operator=(const FfmpegAvioBridge&) = delete;

    [[nodiscard]] bool open(core::decode::ByteSourceBoundary& source) noexcept;
    void close() noexcept;

    [[nodiscard]] AVIOContext* context() const noexcept;
    [[nodiscard]] core::decode::DecodeFailure last_failure() const noexcept;

private:
    static int read_packet(void* opaque, uint8_t* buffer, int buffer_size) noexcept;
    static int64_t seek(void* opaque, int64_t offset, int whence) noexcept;

    core::decode::ByteSourceBoundary* source_{nullptr};
    AVIOContext* context_{nullptr};
    core::decode::DecodeFailure last_failure_{core::decode::DecodeFailure::None};
};

} // namespace kivo::adapters::ffmpeg::detail
