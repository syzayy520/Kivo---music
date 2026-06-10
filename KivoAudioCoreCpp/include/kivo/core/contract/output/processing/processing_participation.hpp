// =============================================================================
// KivoAudioCoreCpp — processing_participation.hpp
// Contract: Which audio processing stages are active
// =============================================================================
//
// Declares whether ReplayGain, Volume, DSP, or ChannelRemix are active.
// No resample status (resample is a format decision, not processing).
// No host audio engine participation (engine is path truth, not processing).
// Platform-neutral. No runtime processing logic.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// ProcessingParticipation — Active audio processing stages
// =============================================================================
struct ProcessingParticipation {
    bool replay_gain_active = false;
    bool volume_active = false;
    bool dsp_active = false;
    bool channel_remix_active = false;

    // --- Convenience ---
    [[nodiscard]] bool any_processing_active() const noexcept {
        return replay_gain_active
            || volume_active
            || dsp_active
            || channel_remix_active;
    }

    [[nodiscard]] bool is_passthrough() const noexcept {
        return !any_processing_active();
    }

    // --- Comparison ---
    [[nodiscard]] bool operator==(const ProcessingParticipation&) const noexcept = default;
};

} // namespace kivo::core::contract
