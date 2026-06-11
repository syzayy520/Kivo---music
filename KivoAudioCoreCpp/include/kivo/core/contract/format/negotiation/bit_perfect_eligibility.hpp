// =============================================================================
// KivoAudioCoreCpp — bit_perfect_eligibility.hpp
// Contract: Whether the audio path can be bit-perfect
// =============================================================================
//
// Determines if the full pipeline from source to device can deliver
// bit-identical audio. Platform-neutral contract — no WASAPI exclusive mode
// or other platform-specific logic.
// =============================================================================

#pragma once

#include <cstdint>

#include "../descriptor/audio_format_descriptor.hpp"

namespace kivo::core::contract {

// =============================================================================
// BitPerfectEligibility — Bit-perfect assessment of the pipeline
// =============================================================================
enum class BitPerfectStatus : uint8_t {
    Unknown = 0,
    Eligible,       // pipeline can be bit-perfect
    Ineligible,     // pipeline cannot be bit-perfect
    BlockedByPolicy // user policy forbids bit-perfect
};

// =============================================================================
// BitPerfectEligibility — Full bit-perfect eligibility report
// =============================================================================
struct BitPerfectEligibility {
    BitPerfectStatus status{BitPerfectStatus::Unknown};
    bool formats_match{false};       // source format == device format
    bool no_conversion_needed{false}; // no sample rate/format conversion
    bool no_mixing_needed{false};    // no channel remixing

    // --- Convenience ---
    [[nodiscard]] constexpr bool is_eligible() const noexcept {
        return status == BitPerfectStatus::Eligible;
    }

    // --- Comparison ---
    [[nodiscard]] constexpr bool operator==(const BitPerfectEligibility&) const noexcept = default;
};

} // namespace kivo::core::contract
