// =============================================================================
// KivoAudioCoreCpp — timeline_clock.hpp
// Contract: User-facing playback position
// =============================================================================
//
// Represents the user-facing playback timeline position.
// No real playback, no UI. Platform-neutral.
// =============================================================================

#pragma once

#include <cstdint>

#include "../../generation_id.hpp"
#include "../../sample_position.hpp"

namespace kivo::core::contract {

// =============================================================================
// TimelineClock — User-facing playback position
// =============================================================================
struct TimelineClock {
    SamplePosition position = kInvalidSamplePosition;
    GenerationId generation = GenerationId::initial();
    bool is_playing = false;

    // --- Validity check ---
    [[nodiscard]] bool is_valid() const noexcept {
        if (position == kInvalidSamplePosition) return false;
        return true;
    }

    // --- Comparison ---
    [[nodiscard]] bool operator==(const TimelineClock&) const noexcept = default;
};

} // namespace kivo::core::contract
