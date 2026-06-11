#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class ErrorDomain : uint8_t {
    Unknown = 0,
    UserError,
    MediaError,
    CodecError,
    FormatError,
    DeviceError,
    BackendError,
    HostError,
    InternalBug,
    DeviceLost,
    FormatChanged,
    CodecStarved,
    BufferUnderrun,
    BufferOverrun,
    UnsupportedFormat,
    SourceUnavailable,
    NetworkInterrupted,
    PermissionDenied,
    CacheInvalid
};

} // namespace kivo::core::contract