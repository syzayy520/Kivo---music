#pragma once

#include <cstdint>

namespace kivo::core::contract {

/// @brief Reader failure mode.
enum class ReaderFailure : uint8_t {
    NotSpecified = 0,
    IoError,
    FormatError,
    PermissionDenied,
    EndOfStream,
    DeviceLost
};

} // namespace kivo::core::contract
