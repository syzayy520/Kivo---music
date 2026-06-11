#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class QueueCapacityPolicy : uint8_t {
    NotSpecified = 0,
    Fixed,
    Elastic,
    Bounded
};

} // namespace kivo::core::contract