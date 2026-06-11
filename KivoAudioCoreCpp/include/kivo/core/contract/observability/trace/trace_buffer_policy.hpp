#pragma once

#include <cstdint>

namespace kivo::core::contract::observability {

enum class TraceBufferPolicy : uint8_t {
    Unknown = 0,
    DropOldest,
    DropNewest,
    Block,
    FixedSize,
    Unbounded
};

} // namespace kivo::core::contract::observability