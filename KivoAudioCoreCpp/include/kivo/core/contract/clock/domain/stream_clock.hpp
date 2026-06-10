// =============================================================================
// KivoAudioCoreCpp — stream_clock.hpp
// Contract: Decoded stream logical clock
// =============================================================================
//
// Represents the decoded stream logical clock position.
// No real decoder, no FFmpeg. Platform-neutral.
// =============================================================================

#pragma once

#include <cstdint>

#include "../../generation_id.hpp"
#include "../../sample_position.hpp"

namespace kivo::core::contract {

// =============================================================================
// StreamClock — Decoded stream logical clock
// =============================================================================
struct StreamClock {
    SamplePosition position = kInvalidSamplePosition;
    GenerationId generation = GenerationId::initial();
    uint32_t sample_rate = 0;

    // --- Validity check ---
    [[nodiscard]] bool is_valid() const noexcept {
        if (position == kInvalidSamplePosition) return false;
        if (sample_rate == 0) return false;
        return true;
    }

    // --- Comparison ---
    [[nodiscard]] bool operator==(const StreamClock&) const noexcept = default;
};

} // namespace kivo::core::contract
