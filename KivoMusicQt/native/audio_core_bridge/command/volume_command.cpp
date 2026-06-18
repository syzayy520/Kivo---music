// =============================================================================
// Kivo Music Qt - volume_command.cpp
// AudioCoreBridge: UI-local volume policy until Host ABI exposes volume
// =============================================================================

#include "volume_command.hpp"

#include "kivo/host/abi/command/kivo_audio_command.h"

#include <algorithm>
#include <cmath>

namespace kivo::qt::audio_bridge::command {

double VolumeCommand::clampVolume(double volume) noexcept {
    return std::clamp(volume, 0.0, 1.0);
}

uint64_t VolumeCommand::toFixedGain(double volume) noexcept {
    const double clamped = clampVolume(volume);
    return static_cast<uint64_t>(std::llround(
        clamped * static_cast<double>(KIVO_AUDIO_VOLUME_GAIN_FIXED_ONE)));
}

} // namespace kivo::qt::audio_bridge::command
