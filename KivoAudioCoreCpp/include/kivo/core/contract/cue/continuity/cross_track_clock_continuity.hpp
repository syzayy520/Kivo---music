// =============================================================================
// KivoAudioCoreCpp — cross_track_clock_continuity.hpp
// Contract: Cross-track clock continuity state
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

    [[nodiscard]] constexpr bool operator==(const CrossTrackClockContinuity&) const noexcept = default;
};

} // namespace kivo::core::contract
