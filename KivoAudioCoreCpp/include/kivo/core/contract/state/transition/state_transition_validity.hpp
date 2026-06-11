#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class StateTransitionValidity : uint8_t {
    Unknown = 0,
    Allowed,
    Forbidden,
    Conditional,
    MergeRequired,
    QueueRequired,
    RejectRequired,
    CompleteCurrentFirst
};

} // namespace kivo::core::contract