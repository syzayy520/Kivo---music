// =============================================================================
// Kivo Music Qt - file_audio_source.cpp
// AudioCoreBridge: Host ABI file source callback lease
// =============================================================================

#include "file_audio_source.hpp"

#include <QFile>
#include <QFileInfo>
#include <algorithm>
#include <memory>

namespace kivo::qt::audio_bridge::source {
namespace {

struct FileAudioSourceContext final {
    explicit FileAudioSourceContext(const QString& path)
        : file(path) {}

    QFile file;
};

kivo_audio_source_io_result readCallback(
    void* sourceContext,
    uint8_t* destination,
    uint64_t capacityBytes,
    uint64_t* bytesRead) {
    if (bytesRead == nullptr) {
        return KIVO_AUDIO_SOURCE_IO_FAILED;
    }
    *bytesRead = 0;
    auto* context = static_cast<FileAudioSourceContext*>(sourceContext);
    if (context == nullptr || destination == nullptr) {
        return KIVO_AUDIO_SOURCE_IO_FAILED;
    }
    if (capacityBytes == 0u) {
        return context->file.atEnd()
            ? KIVO_AUDIO_SOURCE_IO_END_OF_STREAM
            : KIVO_AUDIO_SOURCE_IO_DATA;
    }

    const auto chunkSize = static_cast<qint64>(
        std::min<uint64_t>(capacityBytes, static_cast<uint64_t>(std::numeric_limits<qint64>::max())));
    const qint64 read = context->file.read(
        reinterpret_cast<char*>(destination),
        chunkSize);
    if (read < 0) {
        return KIVO_AUDIO_SOURCE_IO_FAILED;
    }
    *bytesRead = static_cast<uint64_t>(read);
    if (read == 0 && context->file.atEnd()) {
        return KIVO_AUDIO_SOURCE_IO_END_OF_STREAM;
    }
    return KIVO_AUDIO_SOURCE_IO_DATA;
}

kivo_audio_source_io_result seekCallback(
    void* sourceContext,
    int64_t offset,
    kivo_audio_seek_origin origin,
    uint64_t* absolutePosition) {
    auto* context = static_cast<FileAudioSourceContext*>(sourceContext);
    if (context == nullptr || absolutePosition == nullptr) {
        return KIVO_AUDIO_SOURCE_IO_FAILED;
    }

    qint64 target = 0;
    switch (origin) {
    case KIVO_AUDIO_SEEK_ORIGIN_BEGIN:
        target = static_cast<qint64>(offset);
        break;
    case KIVO_AUDIO_SEEK_ORIGIN_CURRENT:
        target = context->file.pos() + static_cast<qint64>(offset);
        break;
    case KIVO_AUDIO_SEEK_ORIGIN_END:
        target = context->file.size() + static_cast<qint64>(offset);
        break;
    default:
        return KIVO_AUDIO_SOURCE_IO_UNSUPPORTED;
    }

    if (target < 0 || !context->file.seek(target)) {
        return KIVO_AUDIO_SOURCE_IO_FAILED;
    }
    *absolutePosition = static_cast<uint64_t>(context->file.pos());
    return KIVO_AUDIO_SOURCE_IO_DATA;
}

void releaseCallback(void* sourceContext) {
    delete static_cast<FileAudioSourceContext*>(sourceContext);
}

} // namespace

error::BridgeResult<FileAudioSourceLease> FileAudioSourceLease::open(
    const QString& filePath,
    uint64_t sourceIdentity,
    uint64_t sourceGeneration) {
    auto context = std::make_unique<FileAudioSourceContext>(filePath);
    if (!context->file.open(QIODevice::ReadOnly)) {
        return error::BridgeResult<FileAudioSourceLease>::failed(
            error::BridgeError::sourceOpenFailed(filePath, -1));
    }

    kivo_audio_source_v1 source{};
    source.struct_size = sizeof(kivo_audio_source_v1);
    source.struct_version = KIVO_AUDIO_ABI_STRUCT_VERSION_1;
    source.source_context = context.release();
    source.source_identity = sourceIdentity;
    source.source_generation = sourceGeneration;
    source.size_bytes = static_cast<uint64_t>(QFileInfo(filePath).size());
    source.flags = KIVO_AUDIO_SOURCE_FLAG_SEEKABLE | KIVO_AUDIO_SOURCE_FLAG_SIZE_KNOWN;
    source.read = readCallback;
    source.seek = seekCallback;
    source.release = releaseCallback;

    return error::BridgeResult<FileAudioSourceLease>::ok(
        FileAudioSourceLease(source));
}

FileAudioSourceLease::FileAudioSourceLease(kivo_audio_source_v1 source) noexcept
    : source_(source) {}

FileAudioSourceLease::~FileAudioSourceLease() {
    releaseOwnedContext();
}

FileAudioSourceLease::FileAudioSourceLease(FileAudioSourceLease&& other) noexcept
    : source_(other.source_), ownsContext_(other.ownsContext_) {
    other.source_ = {};
    other.ownsContext_ = false;
}

FileAudioSourceLease& FileAudioSourceLease::operator=(
    FileAudioSourceLease&& other) noexcept {
    if (this != &other) {
        releaseOwnedContext();
        source_ = other.source_;
        ownsContext_ = other.ownsContext_;
        other.source_ = {};
        other.ownsContext_ = false;
    }
    return *this;
}

const kivo_audio_source_v1& FileAudioSourceLease::abiSource() const noexcept {
    return source_;
}

void FileAudioSourceLease::releaseToHost() noexcept {
    ownsContext_ = false;
}

void FileAudioSourceLease::releaseOwnedContext() noexcept {
    if (ownsContext_ && source_.release != nullptr) {
        source_.release(source_.source_context);
    }
    source_ = {};
    ownsContext_ = false;
}

} // namespace kivo::qt::audio_bridge::source
