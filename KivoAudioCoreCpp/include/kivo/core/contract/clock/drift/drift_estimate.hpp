// =============================================================================
// KivoAudioCoreCpp — drift_estimate.hpp
// Contract: Estimated drift between device and stream clocks
// =============================================================================
//
// Estimates clock drift between device and stream clocks.
// No real drift recovery, no WASAPI. Platform-neutral.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// DriftDirection — Direction of clock drift
// =============================================================================
enum class DriftDirection : uint8_t {
    Unknown = 0,
    None,        // Clocks are synchronized
    DeviceAhead, // Device clock is ahead of stream clock
    StreamAhead  // Stream clock is ahead of device clock
};

// =============================================================================
// DriftEstimate — Estimated drift between clocks
// =============================================================================
struct DriftEstimate {
    DriftDirection direction = DriftDirection::Unknown;
    int64_t drift_samples = 0;   // Positive = device ahead, negative = stream ahead
    uint32_t confidence_pct = 0; // 0-100

    // --- Validity check ---
    [[nodiscard]] bool is_valid() const noexcept {
        if (direction == DriftDirection::Unknown) return false;
        if (confidence_pct > 100) return false;
        return true;
    }

    // --- Significance check ---
    [[nodiscard]] bool is_significant(int64_t threshold) const noexcept {
        if (direction == DriftDirection::Unknown) return false;
        if (direction == DriftDirection::None) return false;
        return (drift_samples > threshold) || (drift_samples < -threshold);
    }

    // --- Comparison ---
    [[nodiscard]] bool operator==(const DriftEstimate&) const noexcept = default;
};

} // namespace kivo::core::contract
