#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class BufferShareSemantics : uint8_t {
    NotSpecified = 0,
    Exclusive,
    SharedReadOnly,
    SharedReadWrite
};

} // namespace kivo::core::contract