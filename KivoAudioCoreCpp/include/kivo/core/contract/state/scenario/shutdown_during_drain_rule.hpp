#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class ShutdownDuringDrainRule : uint8_t {
    Unknown = 0,
    CompleteDrainThenClose,
    AbortDrainThenClose,
    ForceClose,
    MarkClosedIdempotent
};

} // namespace kivo::core::contract