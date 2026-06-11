#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class ReentrantTransitionPolicy : uint8_t {
    Unknown = 0,
    Forbid,
    Merge,
    Queue,
    ReplacePending,
    KeepLatest
};

} // namespace kivo::core::contract