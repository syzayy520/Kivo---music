// =============================================================================
// Kivo Music Qt - volume_command.hpp
// AudioCoreBridge: UI-local volume policy until Host ABI exposes volume
// =============================================================================

#pragma once

namespace kivo::qt::audio_bridge::command {

class VolumeCommand final {
public:
    static double clampVolume(double volume) noexcept;
};

} // namespace kivo::qt::audio_bridge::command
