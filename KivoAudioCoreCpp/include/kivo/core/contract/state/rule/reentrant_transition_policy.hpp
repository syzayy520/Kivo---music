#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class ReentrantTransitionPolicy : uint8_t {
    Unknown = 0,
    Merge,
    Queue,
    Reject,
    Cancel
};

} // namespace kivo::core::contract