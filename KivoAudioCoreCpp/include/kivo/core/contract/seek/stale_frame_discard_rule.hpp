// =============================================================================
// KivoAudioCoreCpp — stale_frame_discard_rule.hpp
// Contract: Stale frame discard rule
// =============================================================================
//
// Defines rules for discarding frames from old generations after seek.
// No real seek runtime. Platform-neutral.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// StaleFrameDiscardRule — How to handle frames from old generation
// =============================================================================
enum class StaleFrameDiscardRule : uint8_t {
    Unknown = 0,
    DiscardAll,         // Discard all frames from old generation (aggressive)
    DiscardAfterSeek,   // Only discard frames after seek position (conservative)
    KeepIfDecoded       // Keep frames already decoded, only discard queued/renderer
};

} // namespace kivo::core::contract
