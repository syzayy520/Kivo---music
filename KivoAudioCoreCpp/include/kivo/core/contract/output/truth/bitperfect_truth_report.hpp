// =============================================================================
// KivoAudioCoreCpp — bitperfect_truth_report.hpp
// Contract: Bit-perfect truth report (actual truth, not eligibility)
// =============================================================================
//
// Expresses whether bit-perfect is actually achieved and reports the
// conditions. Unlike P0-C-004 BitPerfectEligibility (can it be?), this
// reports the actual truth after policy and path evaluation.
// Platform-neutral. No runtime. Consumes P0-C-004 types.
// =============================================================================

#pragma once

#include <cstdint>
#include <optional>

#include "bitperfect_rejection_reason.hpp"
#include "../../format/negotiation/resample_decision.hpp"

namespace kivo::core::contract {

// =============================================================================
// BitPerfectTruthReport — Actual bit-perfect truth assessment
// =============================================================================
struct BitPerfectTruthReport {
    bool bit_perfect_achieved = false;
    bool evidence_complete = false;
    bool requested_exclusive = false;
    bool actual_exclusive = false;
    bool formats_match = false;
    bool no_processing_active = false;
    bool no_engine_in_path = false;
    bool no_sample_mutation = false;
    BitPerfectRejectionReason rejection_reason;
    std::optional<ResampleDecision> resample;

    // --- Convenience ---
    [[nodiscard]] bool is_bit_perfect() const noexcept {
        return bit_perfect_achieved
            && evidence_complete
            && requested_exclusive
            && actual_exclusive
            && formats_match
            && no_processing_active
            && no_engine_in_path
            && no_sample_mutation
            && !rejection_reason.has_reason();
    }

    [[nodiscard]] bool was_rejected() const noexcept {
        return !bit_perfect_achieved && rejection_reason.has_reason();
    }

    // --- Comparison ---
    [[nodiscard]] bool operator==(const BitPerfectTruthReport&) const noexcept = default;
};

} // namespace kivo::core::contract
