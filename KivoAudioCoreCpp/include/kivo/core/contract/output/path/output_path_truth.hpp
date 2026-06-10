// =============================================================================
// KivoAudioCoreCpp — output_path_truth.hpp
// Contract: Aggregated path truth (format match + transparency + engine)
// =============================================================================
//
// Combines device format match, sample transparency, and host engine
// participation into a single path truth. Does NOT include final decision
// or processing flags. Platform-neutral.
// =============================================================================

#pragma once

#include <cstdint>

#include "device_format_match.hpp"
#include "sample_transparency.hpp"
#include "host_audio_engine_participation.hpp"

namespace kivo::core::contract {

// =============================================================================
// OutputPathTruth — Aggregated output path truth
// =============================================================================
struct OutputPathTruth {
    DeviceFormatMatch format_match;
    SampleTransparency transparency;
    HostAudioEngineParticipation engine;

    // --- Convenience ---
    [[nodiscard]] bool is_path_clean() const noexcept {
        return format_match.is_match()
            && transparency.is_transparent()
            && engine.is_bypassing_engine();
    }

    // --- Comparison ---
    [[nodiscard]] bool operator==(const OutputPathTruth&) const noexcept = default;
};

} // namespace kivo::core::contract
