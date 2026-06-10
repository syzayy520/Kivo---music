// =============================================================================
// KivoAudioCoreCpp — output_truth_snapshot.hpp
// Contract: Read-only snapshot of all output truth signals
// =============================================================================
//
// Aggregates truth, processing, path, and decision information into a
// single point-in-time snapshot. No runtime state machine. No mutation.
// Platform-neutral. Consumes P0-C-004 and local sub-family types.
// =============================================================================

#pragma once

#include <cstdint>

#include "bitperfect_truth_report.hpp"
#include "../processing/processing_participation.hpp"
#include "../path/device_format_match.hpp"
#include "../path/sample_transparency.hpp"
#include "../path/host_audio_engine_participation.hpp"
#include "../path/output_path_truth.hpp"
#include "../decision/output_policy_decision.hpp"

namespace kivo::core::contract {

// =============================================================================
// OutputTruthSnapshot — Aggregated output truth at a point in time
// =============================================================================
struct OutputTruthSnapshot {
    BitPerfectTruthReport bitperfect;
    ProcessingParticipation processing;
    DeviceFormatMatch format_match;
    SampleTransparency transparency;
    HostAudioEngineParticipation engine;
    OutputPathTruth path_truth;
    OutputPolicyDecision decision;

    // --- Convenience ---
    [[nodiscard]] bool is_bit_perfect() const noexcept {
        return bitperfect.is_bit_perfect();
    }

    // --- Comparison ---
    [[nodiscard]] bool operator==(const OutputTruthSnapshot&) const noexcept = default;
};

} // namespace kivo::core::contract
