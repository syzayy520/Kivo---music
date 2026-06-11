#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class BufferOwnershipMode : uint8_t {
    NotSpecified = 0,
    Owned,
    Borrowed,
    Shared
};

} // namespace kivo::core::contract