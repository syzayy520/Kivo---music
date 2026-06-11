#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class CloseDuringDrainRule : uint8_t {
    Unknown = 0,
    CancelDrainAndClose,
    WaitForDrain,
    ForceClose,
    RejectClose,
    DeferClose
};

} // namespace kivo::core::contract