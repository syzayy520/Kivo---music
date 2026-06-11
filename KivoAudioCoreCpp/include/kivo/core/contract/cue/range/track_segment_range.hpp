// =============================================================================
// KivoAudioCoreCpp — track_segment_range.hpp
// Contract: Track segment range combining source and timeline ranges
// =============================================================================
//
// P0-C-009 internal helper type. Not a media library track model.
// No real demuxer, no track model.
// =============================================================================

#pragma once

#include <cstdint>
#include "kivo/core/contract/cue/range/source_time_range.hpp"
#include "kivo/core/contract/cue/range/timeline_range.hpp"

namespace kivo::core::contract {

// =============================================================================
// TrackSegmentRange — Internal helper for segment range mapping
// =============================================================================
struct TrackSegmentRange {
    uint32_t segment_index{0};
    SourceTimeRange source_range{};
    TimelineRange timeline_range{};

    [[nodiscard]] bool operator==(const TrackSegmentRange&) const noexcept = default;
};

} // namespace kivo::core::contract
