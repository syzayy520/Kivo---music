#include "host/abi/source/host_callback_byte_source.hpp"

namespace kivo::host::abi {

HostCallbackByteSource::HostCallbackByteSource(
    const kivo_audio_source_v1& source) noexcept
    : source_(source) {}

HostCallbackByteSource::~HostCallbackByteSource() {
    if (source_.release != nullptr) {
        try {
            source_.release(source_.source_context);
        } catch (...) {
        }
    }
}

core::contract::SourceIdentity
HostCallbackByteSource::identity() const noexcept {
    return {source_.source_identity};
}

core::contract::SourceGeneration
HostCallbackByteSource::generation() const noexcept {
    return {
        core::contract::GenerationId::from_value(
            source_.source_generation)
    };
}

bool HostCallbackByteSource::seekable() const noexcept {
    return (source_.flags & KIVO_AUDIO_SOURCE_FLAG_SEEKABLE) != 0u;
}

std::optional<uint64_t>
HostCallbackByteSource::size_bytes() const noexcept {
    if ((source_.flags & KIVO_AUDIO_SOURCE_FLAG_SIZE_KNOWN) == 0u) {
        return std::nullopt;
    }
    return source_.size_bytes;
}

core::decode::ByteSourceReadResult
HostCallbackByteSource::read(
    std::span<std::byte> destination) noexcept {
    if (source_.read == nullptr) {
        return {
            core::decode::ByteSourceReadDisposition::Failed,
            0,
            core::decode::DecodeFailure::SourceReadFailed
        };
    }

    uint64_t bytes_read = 0;
    kivo_audio_source_io_result result =
        KIVO_AUDIO_SOURCE_IO_FAILED;
    try {
        result = source_.read(
            source_.source_context,
            reinterpret_cast<uint8_t*>(destination.data()),
            static_cast<uint64_t>(destination.size()),
            &bytes_read);
    } catch (...) {
        result = KIVO_AUDIO_SOURCE_IO_FAILED;
    }
    if (bytes_read > destination.size()) {
        return {
            core::decode::ByteSourceReadDisposition::Failed,
            0,
            core::decode::DecodeFailure::SourceReadFailed
        };
    }
    if (result == KIVO_AUDIO_SOURCE_IO_DATA && bytes_read != 0u) {
        return {
            core::decode::ByteSourceReadDisposition::Data,
            static_cast<size_t>(bytes_read),
            core::decode::DecodeFailure::None
        };
    }
    if (result == KIVO_AUDIO_SOURCE_IO_END_OF_STREAM) {
        return {
            core::decode::ByteSourceReadDisposition::EndOfStream,
            0,
            core::decode::DecodeFailure::EndOfStream
        };
    }
    return {
        core::decode::ByteSourceReadDisposition::Failed,
        0,
        core::decode::DecodeFailure::SourceReadFailed
    };
}

core::decode::ByteSourceSeekResult
HostCallbackByteSource::seek(
    int64_t offset,
    core::decode::ByteSeekOrigin origin) noexcept {
    if (!seekable() || source_.seek == nullptr) {
        return {
            core::decode::ByteSourceSeekDisposition::Unsupported,
            0,
            core::decode::DecodeFailure::SeekUnsupported
        };
    }

    uint64_t position = 0;
    kivo_audio_source_io_result result =
        KIVO_AUDIO_SOURCE_IO_FAILED;
    try {
        result = source_.seek(
            source_.source_context,
            offset,
            static_cast<kivo_audio_seek_origin>(origin),
            &position);
    } catch (...) {
        result = KIVO_AUDIO_SOURCE_IO_FAILED;
    }
    if (result == KIVO_AUDIO_SOURCE_IO_DATA) {
        return {
            core::decode::ByteSourceSeekDisposition::Succeeded,
            position,
            core::decode::DecodeFailure::None
        };
    }
    if (result == KIVO_AUDIO_SOURCE_IO_UNSUPPORTED) {
        return {
            core::decode::ByteSourceSeekDisposition::Unsupported,
            position,
            core::decode::DecodeFailure::SeekUnsupported
        };
    }
    return {
        core::decode::ByteSourceSeekDisposition::Failed,
        position,
        core::decode::DecodeFailure::SourceSeekFailed
    };
}

} // namespace kivo::host::abi
