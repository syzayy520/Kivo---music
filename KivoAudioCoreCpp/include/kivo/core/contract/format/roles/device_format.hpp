// =============================================================================
// KivoAudioCoreCpp — device_format.hpp
// Contract: Format accepted by the audio output device
// =============================================================================
//
// Represents the PCM format that the audio device actually accepts.
// This is the final format in the pipeline.
// Platform-neutral.
// =============================================================================

#pragma once

#include <cstdint>

#include "../descriptor/audio_format_descriptor.hpp"

namespace kivo::core::contract {

// =============================================================================
// DeviceFormat — Audio device accepted format
// =============================================================================
struct DeviceFormat {
    AudioFormatDescriptor format{};

    // --- Validity ---
    [[nodiscard]] bool is_valid() const noexcept {
        return format.is_valid();
    }

    // --- Comparison ---
    [[nodiscard]] constexpr bool operator==(const DeviceFormat&) const noexcept = default;
};

} // namespace kivo::core::contract
