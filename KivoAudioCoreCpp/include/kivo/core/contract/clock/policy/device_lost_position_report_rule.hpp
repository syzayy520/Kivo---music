// =============================================================================
// KivoAudioCoreCpp — device_lost_position_report_rule.hpp
// Contract: How to report position after device lost
// =============================================================================
//
// Defines position reporting behavior after device loss.
// No real device, no WASAPI. Platform-neutral.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// DeviceLostPositionReportRule — Position reporting after device loss
// =============================================================================
enum class DeviceLostPositionReportRule : uint8_t {
    Unknown = 0,
    LastKnown,      // Report last known device position
    StreamEstimate, // Estimate from stream position
    Zero,           // Report zero
    Invalid         // Report invalid position
};

} // namespace kivo::core::contract
