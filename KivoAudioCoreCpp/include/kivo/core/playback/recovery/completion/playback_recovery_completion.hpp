#pragma once

#include <cstdint>

namespace kivo::core::playback {

enum class PlaybackRecoveryCompletion : uint8_t {
    Restored = 0,
    Ready,
    Stopped,
    Failed
};

} // namespace kivo::core::playback
