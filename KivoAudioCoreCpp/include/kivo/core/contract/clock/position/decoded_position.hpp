// =============================================================================
// KivoAudioCoreCpp — decoded_position.hpp
// Contract: Last decoded sample position
// =============================================================================
//
// Tracks the last decoded sample position and frame count.
// No real decoder, no FFmpeg. Platform-neutral.
// =============================================================================

#pragma once

#include <cstdint>

#include "../../generation_id.hpp"
#include "../../sample_position.hpp"

namespace kivo::core::contract {

// =============================================================================
// DecodedPosition — Last decoded sample position
// =============================================================================
struct DecodedPosition {
    SamplePosition sample = kInvalidSamplePosition;
    FrameCount frames_decoded = 0;
    GenerationId generation = GenerationId::initial();

    // --- Validity check ---
    [[nodiscard]] bool is_valid() const noexcept {
        if (sample == kInvalidSamplePosition) return false;
        return true;
    }

    // --- Comparison ---
    [[nodiscard]] bool operator==(const DecodedPosition&) const noexcept = default;
};

} // namespace kivo::core::contract
