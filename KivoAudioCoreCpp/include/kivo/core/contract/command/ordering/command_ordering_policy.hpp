#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class CommandOrderingPolicy : uint8_t {
    Unknown = 0,
    Fifo,
    Lifo,
    PriorityOrdered,
    DeadlineOrdered,
    None
};

} // namespace kivo::core::contract