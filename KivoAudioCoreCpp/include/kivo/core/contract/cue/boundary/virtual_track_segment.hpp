// =============================================================================
// KivoAudioCoreCpp — virtual_track_segment.hpp
// Contract: Virtual track segment boundary definition
// =============================================================================
//
// Platform-neutral virtual track segment reservation.
// No real demuxer, no track model.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// VirtualTrackSegment — Virtual track segment boundary definition
// =============================================================================
struct VirtualTrackSegment {
    uint32_t segment_index{0};
    uint64_t start_sample{0};
    uint64_t end_sample{0};
    uint32_t parent_track_count{0};

    [[nodiscard]] bool operator==(const VirtualTrackSegment&) const noexcept = default;
};

} // namespace kivo::core::contract
