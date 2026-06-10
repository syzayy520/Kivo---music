// =============================================================================
// KivoAudioCoreCpp — seek_target.hpp
// Contract: Seek target (resolved seek position)
// =============================================================================
//
// Defines the resolved seek target with position, generation, and accuracy.
// No real seek runtime. Platform-neutral.
// =============================================================================

#pragma once

#include "../sample_position.hpp"
#include "../generation_id.hpp"
#include "seek_flush.hpp"

namespace kivo::core::contract {

// =============================================================================
// SeekTarget — Resolved seek position
// =============================================================================
struct SeekTarget {
    SamplePosition position = kInvalidSamplePosition;
    GenerationId generation = GenerationId::initial();
    SeekAccuracy accuracy = SeekAccuracy::Exact;

    // --- Validity check ---
    [[nodiscard]] bool is_valid() const noexcept {
        return position != kInvalidSamplePosition;
    }

    // --- Comparison ---
    [[nodiscard]] bool operator==(const SeekTarget&) const noexcept = default;
};

} // namespace kivo::core::contract
