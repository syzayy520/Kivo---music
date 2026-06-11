#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class PauseDuringTrackSwitchRule : uint8_t {
    Unknown = 0,
    QueueAfterSwitch,
    CancelSwitch,
    PauseImmediately,
    RejectPause,
    DeferPause
};

} // namespace kivo::core::contract