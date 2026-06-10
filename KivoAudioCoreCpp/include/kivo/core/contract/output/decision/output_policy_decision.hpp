// =============================================================================
// KivoAudioCoreCpp — output_policy_decision.hpp
// Contract: Final output policy decision
// =============================================================================
//
// Expresses the final output policy decision after truth evaluation.
// Does NOT define OutputMode (P0-C-004 already defines it).
// Does NOT implement runtime output route.
// Platform-neutral.
// =============================================================================

#pragma once

#include <cstdint>

#include "output_policy_explanation.hpp"
#include "../../format/negotiation/negotiated_output_mode.hpp"

namespace kivo::core::contract {

// =============================================================================
// OutputPolicyDecision — Final output policy decision
// =============================================================================
//
// Consumes P0-C-004 NegotiatedOutputMode (the negotiation result) and
// adds the truth-level assessment. Does NOT redefine OutputMode.
// =============================================================================
struct OutputPolicyDecision {
    OutputMode mode = OutputMode::Unknown;
    bool bit_perfect_active = false;
    OutputPolicyExplanation explanation;

    // --- Convenience ---
    [[nodiscard]] bool is_valid() const noexcept {
        return mode != OutputMode::Unknown;
    }

    [[nodiscard]] bool is_bit_perfect() const noexcept {
        return bit_perfect_active;
    }

    // --- Comparison ---
    [[nodiscard]] bool operator==(const OutputPolicyDecision&) const noexcept = default;
};

} // namespace kivo::core::contract
