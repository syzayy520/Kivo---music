// =============================================================================
// KivoAudioCoreCpp — rendered_position.hpp
// Contract: Last submitted to device position
// =============================================================================
//
// Tracks the last rendered sample position and frame count.
// No real device buffer, no WASAPI. Platform-neutral.
// =============================================================================

#pragma once

#include <cstdint>

#include "../generation_id.hpp"
#include "../sample_position.hpp"

namespace kivo::core::contract {

// =============================================================================
// RenderedPosition — Last submitted to device position
// =============================================================================
struct RenderedPosition {
    SamplePosition sample = kInvalidSamplePosition;
    FrameCount frames_written = 0;
    GenerationId generation = GenerationId::initial();

    // --- Validity check ---
    [[nodiscard]] bool is_valid() const noexcept {
        if (sample == kInvalidSamplePosition) return false;
        return true;
    }

    // --- Comparison ---
    [[nodiscard]] bool operator==(const RenderedPosition&) const noexcept = default;
};

} // namespace kivo::core::contract
