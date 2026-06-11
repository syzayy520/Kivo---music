#pragma once

#include <cstdint>

namespace kivo::core::contract {

/// @brief Source boundary mode — how the source boundary operates.
enum class SourceBoundaryMode : uint8_t {
    NotSpecified = 0,
    Synchronous,
    Asynchronous,
    Streaming,
    BoundedView
};

} // namespace kivo::core::contract
