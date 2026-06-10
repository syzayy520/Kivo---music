// =============================================================================
// KivoAudioCoreCpp — bitperfect_rejection_reason.hpp
// Contract: Structured reason why bit-perfect was rejected
// =============================================================================
//
// Provides a platform-neutral explanation for bit-perfect failure.
// No runtime device diagnostics. No platform API dependency.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// BitPerfectRejectionCategory — High-level rejection category
// =============================================================================
enum class BitPerfectRejectionCategory : uint8_t {
    Unknown = 0,
    FormatMismatch,       // source and device formats differ
    SampleRateMismatch,   // source and device sample rates differ
    ChannelLayoutMismatch,// source and device channel layouts differ
    ProcessingActive,     // DSP/volume/replay-gain is active
    EngineParticipation,   // host audio engine is in the path
    PolicyForbidden        // bit-perfect disabled by user policy
};

// =============================================================================
// BitPerfectRejectionReason — Structured rejection reason
// =============================================================================
struct BitPerfectRejectionReason {
    BitPerfectRejectionCategory category = BitPerfectRejectionCategory::Unknown;
    bool format_mismatch = false;
    bool sample_rate_mismatch = false;
    bool channel_layout_mismatch = false;
    bool processing_active = false;
    bool engine_in_path = false;
    bool policy_forbidden = false;

    // --- Convenience ---
    [[nodiscard]] bool has_reason() const noexcept {
        return category != BitPerfectRejectionCategory::Unknown;
    }

    // --- Comparison ---
    [[nodiscard]] bool operator==(const BitPerfectRejectionReason&) const noexcept = default;
};

} // namespace kivo::core::contract
