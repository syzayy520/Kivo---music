#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class BufferReferenceType : uint8_t {
    NotSpecified = 0,
    Strong,
    Weak,
    Observer
};

} // namespace kivo::core::contract