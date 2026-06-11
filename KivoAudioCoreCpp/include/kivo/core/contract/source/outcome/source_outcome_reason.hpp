#pragma once

#include <cstdint>

namespace kivo::core::contract {

/// @brief Source outcome reason — why a source operation succeeded or failed.
enum class SourceOutcomeReason : uint8_t {
    NotSpecified = 0,
    Success,
    IoFailure,
    FormatFailure,
    PermissionFailure,
    Timeout,
    Cancelled,
    DeviceLost
};

} // namespace kivo::core::contract
