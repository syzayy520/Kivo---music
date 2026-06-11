#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class CoreState : uint8_t {
    Unknown = 0,
    Idle,
    Opening,
    Ready,
    Playing,
    Pausing,
    Paused,
    Seeking,
    Draining,
    Recovering,
    Stopped,
    Failed,
    Closed
};

} // namespace kivo::core::contract