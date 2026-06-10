// =============================================================================
// KivoAudioCoreCpp — output_policy_explanation.hpp
// Contract: Explanation of why the output policy decision was made
// =============================================================================
//
// Describes the rationale behind an output policy decision.
// Platform-neutral. No runtime logging/reporting. No UI strings.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// OutputPolicyExplanation — Decision rationale
// =============================================================================
struct OutputPolicyExplanation {
    bool bit_perfect_applicable = false;
    bool bit_perfect_achieved = false;
    bool format_conversion_performed = false;
    bool resample_performed = false;
    bool processing_active = false;
    bool engine_in_path = false;

    // --- Convenience ---
    [[nodiscard]] bool bit_perfect_blocked() const noexcept {
        return bit_perfect_applicable && !bit_perfect_achieved;
    }

    // --- Comparison ---
    [[nodiscard]] bool operator==(const OutputPolicyExplanation&) const noexcept = default;
};

} // namespace kivo::core::contract
