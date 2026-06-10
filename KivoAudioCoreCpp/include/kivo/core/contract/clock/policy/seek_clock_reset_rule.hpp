// =============================================================================
// KivoAudioCoreCpp — seek_clock_reset_rule.hpp
// Contract: How clocks reset after seek
// =============================================================================
//
// Defines clock reset behavior after seek operations.
// No real seek, no decoder. Platform-neutral.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// SeekClockResetRule — Clock reset behavior after seek
// =============================================================================
enum class SeekClockResetRule : uint8_t {
    Unknown = 0,
    ResetAll,          // Reset all clocks to seek target
    ResetDevice,       // Reset device clock only
    ResetStream,       // Reset stream clock only
    ResetTimeline,     // Reset timeline only
    IncrementGeneration // Increment generation without resetting position
};

} // namespace kivo::core::contract
