// =============================================================================
// KivoAudioCoreCpp — negotiated_output_mode.hpp
// Contract: Output mode resulting from format negotiation
// =============================================================================
//
// Describes the output mode chosen by the format negotiation process.
// This is strictly a format negotiation outcome — it does NOT express
// output route, device runtime policy, or any runtime behavior.
// Platform-neutral. No WASAPI shared/exclusive mode dependency.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// NegotiatedOutputMode — Output mode determined by format negotiation
// =============================================================================
//
// This type ONLY describes the result of format negotiation.
// It does NOT control or describe audio output device runtime behavior.
// =============================================================================
enum class OutputMode : uint8_t {
    Unknown = 0,
    Shared,              // shared-mode output (Windows-style shared)
    Exclusive,           // exclusive-mode output (bit-perfect capable)
    BitPerfectRequested,  // user requested bit-perfect
    BitPerfectActive,     // bit-perfect confirmed active by negotiation
    BitPerfectRejected,   // bit-perfect requested but rejected
    BitPerfectUnavailable // device does not support bit-perfect
};

// =============================================================================
// NegotiatedOutputMode — Format negotiation output mode result
// =============================================================================
struct NegotiatedOutputMode {
    OutputMode mode = OutputMode::Unknown;
    bool bit_perfect_active = false;
    bool resampling_active = false;
    bool conversion_active = false;

    // --- Convenience ---
    [[nodiscard]] bool is_bit_perfect() const noexcept {
        return bit_perfect_active && !resampling_active && !conversion_active;
    }

    [[nodiscard]] bool is_valid() const noexcept {
        return mode != OutputMode::Unknown;
    }

    // --- Comparison ---
    [[nodiscard]] bool operator==(const NegotiatedOutputMode&) const noexcept = default;
};

} // namespace kivo::core::contract
