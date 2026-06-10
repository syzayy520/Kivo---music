// =============================================================================
// KivoAudioCoreCpp — pause_resume_freeze_policy.hpp
// Contract: What happens to clocks during pause/resume
// =============================================================================
//
// Defines clock behavior during pause/resume transitions.
// No real playback, no WASAPI. Platform-neutral.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// PauseResumeFreezePolicy — Clock freeze behavior during pause/resume
// =============================================================================
enum class PauseResumeFreezePolicy : uint8_t {
    Unknown = 0,
    FreezeAll,      // All clocks freeze on pause, resume from frozen position
    FreezeDevice,   // Device clock freezes, stream clock continues
    FreezeTimeline, // Timeline freezes, device/stream continue
    None            // No freezing (clocks continue running)
};

} // namespace kivo::core::contract
