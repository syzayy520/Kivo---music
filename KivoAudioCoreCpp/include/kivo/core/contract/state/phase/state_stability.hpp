#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class StateStability : uint8_t {
    Unknown = 0,
    Stable,
    Transitioning,
    Unstable
};

} // namespace kivo::core::contract