#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class BufferOwnershipTransfer : uint8_t {
    NotSpecified = 0,
    Move,
    Copy,
    Share,
    Delegate
};

} // namespace kivo::core::contract