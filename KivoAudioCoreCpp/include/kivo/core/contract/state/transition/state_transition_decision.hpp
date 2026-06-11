#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class StateTransitionDecision : uint8_t {
    Unknown = 0,
    Allow,
    Reject,
    Queue,
    Merge,
    Defer,
    Conditional
};

} // namespace kivo::core::contract