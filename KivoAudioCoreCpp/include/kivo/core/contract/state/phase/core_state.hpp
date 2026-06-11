#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class CoreState : uint8_t {
    Unknown = 0,
    Idle,
    Preparing,
    Ready,
    Playing,
    Paused,
    Seeking,
    Draining,
    Flushing,
    Closing,
    Closed,
    Failed,
    Recovering
};

} // namespace kivo::core::contract