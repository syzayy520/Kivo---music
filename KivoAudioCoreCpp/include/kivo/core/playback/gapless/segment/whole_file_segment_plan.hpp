#pragma once

#include <cstdint>
#include <span>

#include "kivo/core/contract/cue/boundary/virtual_track_segment.hpp"
#include "kivo/core/contract/cue/range/track_segment_range.hpp"
#include "kivo/core/contract/sample_position.hpp"

namespace kivo::core::playback {

enum class WholeFileSegmentPlanDisposition : uint8_t {
    Planned = 0,
    InvalidRequest,
    InvalidSegment,
    NonContiguousSource,
    IncompleteSourceCoverage,
    InsufficientOutput,
    TimelineOverflow
};

struct WholeFileSegmentPlanRequest {
    uint64_t source_ref{0};
    uint32_t sample_rate{0};
    contract::FrameCount source_frames{0};
    contract::SamplePosition timeline_origin{0};

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return source_ref != 0
            && sample_rate != 0
            && source_frames != 0
            && timeline_origin != contract::kInvalidSamplePosition;
    }
};

struct WholeFileSegmentPlanResult {
    WholeFileSegmentPlanDisposition disposition{
        WholeFileSegmentPlanDisposition::InvalidRequest};
    size_t planned_segments{0};
};

[[nodiscard]] WholeFileSegmentPlanResult plan_whole_file_segments(
    const WholeFileSegmentPlanRequest& request,
    std::span<const contract::VirtualTrackSegment> segments,
    std::span<contract::TrackSegmentRange> output) noexcept;

} // namespace kivo::core::playback
