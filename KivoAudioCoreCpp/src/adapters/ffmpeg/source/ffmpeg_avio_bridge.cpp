#include "source/ffmpeg_avio_bridge.hpp"

#include <algorithm>
#include <cstddef>
#include <limits>
#include <span>

extern "C" {
#include <libavutil/error.h>
#include <libavutil/mem.h>
}

namespace kivo::adapters::ffmpeg::detail {

namespace {

constexpr int kAvioBufferSize = 64 * 1024;

} // namespace

FfmpegAvioBridge::~FfmpegAvioBridge() {
    close();
}

bool FfmpegAvioBridge::open(
    core::decode::ByteSourceBoundary& source) noexcept {
    close();
    auto* buffer = static_cast<unsigned char*>(av_malloc(kAvioBufferSize));
    if (buffer == nullptr) {
        last_failure_ = core::decode::DecodeFailure::BoundaryFailure;
        return false;
    }

    source_ = &source;
    context_ = avio_alloc_context(
        buffer,
        kAvioBufferSize,
        0,
        this,
        &FfmpegAvioBridge::read_packet,
        nullptr,
        source.seekable() ? &FfmpegAvioBridge::seek : nullptr);
    if (context_ == nullptr) {
        av_free(buffer);
        source_ = nullptr;
        last_failure_ = core::decode::DecodeFailure::BoundaryFailure;
        return false;
    }
    context_->seekable = source.seekable() ? AVIO_SEEKABLE_NORMAL : 0;
    last_failure_ = core::decode::DecodeFailure::None;
    return true;
}

void FfmpegAvioBridge::close() noexcept {
    if (context_ != nullptr) {
        av_freep(&context_->buffer);
        avio_context_free(&context_);
    }
    source_ = nullptr;
    last_failure_ = core::decode::DecodeFailure::None;
}

AVIOContext* FfmpegAvioBridge::context() const noexcept {
    return context_;
}

core::decode::DecodeFailure FfmpegAvioBridge::last_failure() const noexcept {
    return last_failure_;
}

int FfmpegAvioBridge::read_packet(
    void* opaque,
    uint8_t* buffer,
    int buffer_size) noexcept {
    auto* bridge = static_cast<FfmpegAvioBridge*>(opaque);
    if (bridge == nullptr
        || bridge->source_ == nullptr
        || buffer == nullptr
        || buffer_size <= 0) {
        return AVERROR(EINVAL);
    }

    const auto requested = static_cast<size_t>(buffer_size);
    const auto result = bridge->source_->read({
        reinterpret_cast<std::byte*>(buffer),
        requested
    });
    using core::decode::ByteSourceReadDisposition;
    switch (result.disposition) {
    case ByteSourceReadDisposition::Data:
        if (result.bytes_read == 0
            || result.bytes_read > requested
            || result.bytes_read
                > static_cast<size_t>(std::numeric_limits<int>::max())) {
            bridge->last_failure_ =
                core::decode::DecodeFailure::SourceReadFailed;
            return AVERROR(EIO);
        }
        return static_cast<int>(result.bytes_read);
    case ByteSourceReadDisposition::EndOfStream:
        return AVERROR_EOF;
    case ByteSourceReadDisposition::Failed:
        bridge->last_failure_ = result.failure;
        return AVERROR(EIO);
    }
    bridge->last_failure_ = core::decode::DecodeFailure::SourceReadFailed;
    return AVERROR(EIO);
}

int64_t FfmpegAvioBridge::seek(
    void* opaque,
    int64_t offset,
    int whence) noexcept {
    auto* bridge = static_cast<FfmpegAvioBridge*>(opaque);
    if (bridge == nullptr || bridge->source_ == nullptr) {
        return AVERROR(EINVAL);
    }
    if ((whence & AVSEEK_SIZE) != 0) {
        const auto size = bridge->source_->size_bytes();
        if (!size.has_value()
            || *size > static_cast<uint64_t>(std::numeric_limits<int64_t>::max())) {
            return AVERROR(ENOSYS);
        }
        return static_cast<int64_t>(*size);
    }

    const auto normalized_whence = whence & ~AVSEEK_FORCE;
    core::decode::ByteSeekOrigin origin{};
    switch (normalized_whence) {
    case SEEK_SET:
        origin = core::decode::ByteSeekOrigin::Begin;
        break;
    case SEEK_CUR:
        origin = core::decode::ByteSeekOrigin::Current;
        break;
    case SEEK_END:
        origin = core::decode::ByteSeekOrigin::End;
        break;
    default:
        return AVERROR(EINVAL);
    }

    const auto result = bridge->source_->seek(offset, origin);
    using core::decode::ByteSourceSeekDisposition;
    if (result.disposition != ByteSourceSeekDisposition::Succeeded
        || result.position
            > static_cast<uint64_t>(std::numeric_limits<int64_t>::max())) {
        bridge->last_failure_ = result.failure;
        return result.disposition == ByteSourceSeekDisposition::Unsupported
            ? AVERROR(ENOSYS)
            : AVERROR(EIO);
    }
    return static_cast<int64_t>(result.position);
}

} // namespace kivo::adapters::ffmpeg::detail
