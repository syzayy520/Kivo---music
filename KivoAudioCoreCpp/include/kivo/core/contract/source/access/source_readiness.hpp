#pragma once

#include <cstdint>

namespace kivo::core::contract {

/// @brief Source readiness — whether the source is ready for reading.
enum class SourceReadiness : uint8_t {
    NotSpecified = 0,
    Unknown,
    Ready,
    NotReady,
    Failed
};

} // namespace kivo::core::contract
