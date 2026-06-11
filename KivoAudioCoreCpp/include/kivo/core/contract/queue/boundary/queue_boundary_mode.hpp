#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class QueueBoundaryMode : uint8_t {
    NotSpecified = 0,
    SPSC,
    MPSC,
    SPMC,
    MPMC
};

} // namespace kivo::core::contract