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
    ForceClose
};

} // namespace kivo::core::contract