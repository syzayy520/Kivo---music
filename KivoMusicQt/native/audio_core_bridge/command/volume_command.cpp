// =============================================================================
// Kivo Music Qt - volume_command.cpp
// AudioCoreBridge: UI-local volume policy until Host ABI exposes volume
// =============================================================================

#include "volume_command.hpp"

#include <algorithm>

namespace kivo::qt::audio_bridge::command {

double VolumeCommand::clampVolume(double volume) noexcept {
    return std::clamp(volume, 0.0, 1.0);
}

} // namespace kivo::qt::audio_bridge::command
