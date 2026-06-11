#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class SafeCleanupPath : uint8_t {
    None = 0,
    GracefulDrain,
    ImmediateFlush,
    ForceClose
};

} // namespace kivo::core::contract