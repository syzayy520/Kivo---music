// =============================================================================
// KivoAudioCoreCpp — host_audio_engine_participation.hpp
// Contract: Whether the host audio engine participates in the output path
// =============================================================================
//
// Expresses whether the platform's audio engine is in the output signal path.
// Naming is Host to keep the contract platform-neutral.
// Belongs to path/ truth family (engine participation is a path property).
// Platform-neutral. No runtime engine interaction.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// HostAudioEngineParticipation — Host audio engine in output path
// =============================================================================
struct HostAudioEngineParticipation {
    bool engine_in_path = false;

    // --- Convenience ---
    [[nodiscard]] bool is_bypassing_engine() const noexcept {
        return !engine_in_path;
    }

    // --- Comparison ---
    [[nodiscard]] bool operator==(const HostAudioEngineParticipation&) const noexcept = default;
};

} // namespace kivo::core::contract
