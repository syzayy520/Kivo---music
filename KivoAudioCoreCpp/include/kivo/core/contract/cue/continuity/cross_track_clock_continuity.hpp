// =============================================================================
// KivoAudioCoreCpp — cross_track_clock_continuity.hpp
// Contract: Cross-track clock continuity state
// =============================================================================
//
// Platform-neutral cross-track clock continuity reservation.
// No P0-C-002 clock include, no real playback, no crossfade.
// All fields are opaque numeric values.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// CrossTrackClockContinuity — Cross-track clock continuity state
// =============================================================================
struct CrossTrackClockContinuity {
    uint64_t previous_track_end_sample{0};
    uint64_t next_track_start_sample{0};
    uint64_t accumulated_offset_samples{0};
    bool continuity_valid{false};

    [[nodiscard]] bool operator==(const CrossTrackClockContinuity&) const noexcept = default;
};

} // namespace kivo::core::contract
