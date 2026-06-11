#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class BufferReusePolicy : uint8_t {
    NotSpecified = 0,
    SingleUse,
    Reusable,
    PoolManaged
};

} // namespace kivo::core::contract