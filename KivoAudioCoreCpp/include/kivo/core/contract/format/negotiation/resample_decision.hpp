// =============================================================================
// KivoAudioCoreCpp — resample_decision.hpp
// Contract: Resampling decision and rationale
// =============================================================================
//
// Captures whether resampling is needed and why.
// Platform-neutral. No SRC / libsamplerate dependency.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// ResampleReason — Why resampling is (or is not) needed
// =============================================================================
enum class ResampleReason : uint8_t {
    NotNeeded = 0,         // formats match
    DeviceMismatch,         // device doesn't support source rate
    ForbiddenByPolicy,      // resample disabled by user policy
    RequiredByPolicy        // resample enforced by user policy
};

// =============================================================================
// ResampleDecision — Whether to resample and why
// =============================================================================
struct ResampleDecision {
    bool resample{false};
    ResampleReason reason{ResampleReason::NotNeeded};
    uint32_t source_rate{0};
    uint32_t target_rate{0};

    // --- Convenience ---
    [[nodiscard]] constexpr bool is_needed() const noexcept {
        return resample;
    }

    // --- Comparison ---
    [[nodiscard]] constexpr bool operator==(const ResampleDecision&) const noexcept = default;
};

} // namespace kivo::core::contract
