#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class BufferOwnership : uint8_t {
    NotSpecified = 0,
    ExplicitRequired,
    ImplicitAllowed
};

} // namespace kivo::core::contract