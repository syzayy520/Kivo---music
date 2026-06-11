// =============================================================================
// KivoAudioCoreCpp — render_format.hpp
// Contract: Format submitted to the audio render boundary
// =============================================================================
//
// Represents the PCM format of audio data delivered to the audio render
// boundary (audio device buffer). This is strictly an audio format type.
// Platform-neutral. No platform audio API dependency.
// =============================================================================

#pragma once

#include <cstdint>

#include "../descriptor/audio_format_descriptor.hpp"

namespace kivo::core::contract {

// =============================================================================
// RenderFormat — Audio render boundary format
// =============================================================================
struct RenderFormat {
    AudioFormatDescriptor format;

    // --- Validity ---
    [[nodiscard]] bool is_valid() const noexcept {
        return format.is_valid();
    }

    // --- Comparison ---
    [[nodiscard]] constexpr bool operator==(const RenderFormat&) const noexcept = default;
};

} // namespace kivo::core::contract
