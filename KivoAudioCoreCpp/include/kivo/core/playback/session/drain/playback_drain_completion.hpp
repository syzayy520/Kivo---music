#pragma once

#include <cstdint>

namespace kivo::core::playback {

enum class PlaybackDrainCompletion : uint8_t {
    None = 0,
    Succeeded,
    Failed
};

} // namespace kivo::core::playback
