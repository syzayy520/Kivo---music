// =============================================================================
// KivoAudioCoreCpp — gapless_timeline_continuity_rule.hpp
// Contract: Timeline continuity across tracks (P0-L prep)
// =============================================================================
//
// Defines timeline continuity behavior for gapless playback.
// No real gapless, no P0-L implementation. Platform-neutral.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// GaplessTimelineContinuityRule — Timeline continuity across tracks
// =============================================================================
enum class GaplessTimelineContinuityRule : uint8_t {
    Unknown = 0,
    Continuous,     // Timeline continues seamlessly across tracks
    ResetPerTrack,  // Timeline resets for each track
    OffsetPerTrack  // Timeline applies offset per track
};

} // namespace kivo::core::contract
