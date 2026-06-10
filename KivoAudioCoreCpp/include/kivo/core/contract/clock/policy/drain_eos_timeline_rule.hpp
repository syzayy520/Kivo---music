// =============================================================================
// KivoAudioCoreCpp — drain_eos_timeline_rule.hpp
// Contract: Timeline behavior at end-of-stream
// =============================================================================
//
// Defines timeline behavior when reaching end of stream.
// No real playback, no drain logic. Platform-neutral.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// DrainEosTimelineRule — Timeline behavior at end-of-stream
// =============================================================================
enum class DrainEosTimelineRule : uint8_t {
    Unknown = 0,
    HoldAtEnd,       // Timeline holds at end position
    ResetToStart,    // Timeline resets to start
    ContinueRunning  // Timeline continues past end
};

} // namespace kivo::core::contract
