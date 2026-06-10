// =============================================================================
// KivoAudioCoreCpp — seek_anti_glitch_policy.hpp
// Contract: Seek anti-glitch policy
// =============================================================================
//
// Defines policies to prevent audio glitches during seek operations.
// No real seek runtime. Platform-neutral.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// SeekAntiGlitchPolicy — How to handle audio during seek
// =============================================================================
enum class SeekAntiGlitchPolicy : uint8_t {
    Unknown = 0,
    MuteDuringSeek,     // Mute output during seek (simple, may cause silence gap)
    FadeOutFadeIn,      // Fade out before seek, fade in after (smooth but adds latency)
    Crossfade,          // Crossfade old and new audio (smooth, higher CPU cost)
    None                // No anti-glitch (may cause pops/clicks)
};

} // namespace kivo::core::contract
