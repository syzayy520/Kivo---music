#pragma once

#include <cstdint>

namespace kivo::core::playback {

enum class PlaybackCommandFailure : uint8_t {
    None = 0,
    InvalidCommand,
    OutOfOrderCommand,
    StaleSession,
    IllegalState,
    UnsupportedCommand,
    SessionClosed
};

} // namespace kivo::core::playback
