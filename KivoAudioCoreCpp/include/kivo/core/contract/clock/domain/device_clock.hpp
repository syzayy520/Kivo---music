// =============================================================================
// KivoAudioCoreCpp — device_clock.hpp
// Contract: Hardware sample clock abstraction
// =============================================================================
//
// Represents the hardware device clock position.
// No real IAudioClock, no WASAPI. Platform-neutral.
// =============================================================================

#pragma once

#include <cstdint>

#include "../../generation_id.hpp"
#include "../../sample_position.hpp"

namespace kivo::core::contract {

// =============================================================================
// DeviceClock — Hardware sample clock position
// =============================================================================
struct DeviceClock {
    SamplePosition position = kInvalidSamplePosition;
    GenerationId generation = GenerationId::initial();
    uint64_t timestamp_qpc = 0;  // Platform-neutral monotonic timestamp

    // --- Validity check ---
    [[nodiscard]] bool is_valid() const noexcept {
        if (position == kInvalidSamplePosition) return false;
        if (timestamp_qpc == 0) return false;
        return true;
    }

    // --- Comparison ---
    [[nodiscard]] bool operator==(const DeviceClock&) const noexcept = default;
};

} // namespace kivo::core::contract
