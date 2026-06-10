// =============================================================================
// KivoAudioCoreCpp — clock_domain.hpp
// Contract: Clock domain and snapshot (minimal)
// =============================================================================
//
// Defines clock domains and snapshots for position tracking.
// No real device clock, no drift recovery. Platform-neutral.
// =============================================================================

#pragma once

#include <cstdint>

#include "../generation_id.hpp"
#include "../sample_position.hpp"

namespace kivo::core::contract {

// =============================================================================
// ClockDomain — Logical clock source
// =============================================================================
enum class ClockDomain : uint8_t {
    Unknown = 0,
    Device,    // Hardware sample clock
    Stream,    // Decoded stream logical clock
    Timeline   // User-facing playback position
};

// =============================================================================
// ClockSnapshot — Point-in-time position in a clock domain
// =============================================================================
struct ClockSnapshot {
    ClockDomain domain = ClockDomain::Unknown;
    SamplePosition position = kInvalidSamplePosition;
    GenerationId generation = GenerationId::initial();

    // --- Validity check ---
    [[nodiscard]] bool is_valid() const noexcept {
        if (domain == ClockDomain::Unknown) return false;
        if (position == kInvalidSamplePosition) return false;
        return true;
    }

    // --- Comparison ---
    [[nodiscard]] bool operator==(const ClockSnapshot&) const noexcept = default;
};

} // namespace kivo::core::contract
