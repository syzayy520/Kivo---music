#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class QueueSlotState : uint8_t {
    NotSpecified = 0,
    Empty,
    Filled,
    Consumed,
    Reserved
};

} // namespace kivo::core::contract