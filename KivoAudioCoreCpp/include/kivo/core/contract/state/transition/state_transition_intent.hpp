#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class StateTransitionIntent : uint8_t {
    Unknown = 0,
    Open,
    Prepare,
    Start,
    Pause,
    Resume,
    Seek,
    Drain,
    Recover,
    Stop,
    Close,
    Fail,
    Reset
};

} // namespace kivo::core::contract