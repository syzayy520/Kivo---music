#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class QueueOverflowPolicy : uint8_t {
    NotSpecified = 0,
    DropOldest,
    DropNewest,
    BlockProducer,
    Reject
};

} // namespace kivo::core::contract