// =============================================================================
// Kivo Music Qt - volume_command.hpp
// AudioCoreBridge: software-volume encoding for the Host ABI SET_VOLUME command
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::qt::audio_bridge::command {

class VolumeCommand final {
public:
    // Clamp a UI volume to the engine's accepted [0, 1] linear-gain range.
    static double clampVolume(double volume) noexcept;

    // Encode a 0..1 linear gain as the Host ABI Q40.24 fixed-point value carried
    // in a SET_VOLUME command's `requested_frame` field (gain * (1 << 24)).
    // Clamped first, so the result never exceeds KIVO_AUDIO_VOLUME_GAIN_FIXED_ONE.
    static uint64_t toFixedGain(double volume) noexcept;
};

} // namespace kivo::qt::audio_bridge::command
