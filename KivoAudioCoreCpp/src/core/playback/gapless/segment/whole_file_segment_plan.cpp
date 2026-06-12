#include "kivo/core/playback/gapless/segment/whole_file_segment_plan.hpp"

#include <limits>

namespace kivo::core::playback {

WholeFileSegmentPlanResult plan_whole_file_segments(
    const WholeFileSegmentPlanRequest& request,
    std::span<const contract::VirtualTrackSegment> segments,
    std::span<contract::TrackSegmentRange> output) noexcept {
    if (!request.is_valid() || segments.empty()) {
        return {WholeFileSegmentPlanDisposition::InvalidRequest, 0};
    }
    if (output.size() < segments.size()) {
        return {WholeFileSegmentPlanDisposition::InsufficientOutput, 0};
    }

    contract::FrameCount expected_source_start{0};
    contract::SamplePosition timeline_cursor{request.timeline_origin};
    for (size_t index = 0; index < segments.size(); ++index) {
        const auto& segment = segments[index];
        if (segment.segment_index != index
            || segment.parent_track_count != segments.size()
            || segment.start_sample >= segment.end_sample
            || segment.end_sample > request.source_frames) {
            return {WholeFileSegmentPlanDisposition::InvalidSegment, 0};
        }
        if (segment.start_sample != expected_source_start) {
            return {
                WholeFileSegmentPlanDisposition::NonContiguousSource,
                0};
        }
        const auto length = segment.end_sample - segment.start_sample;
        if (length
            > std::numeric_limits<contract::SamplePosition>::max()
                - timeline_cursor) {
            return {WholeFileSegmentPlanDisposition::TimelineOverflow, 0};
        }
        output[index] = {
            segment.segment_index,
            {
                request.source_ref,
                segment.start_sample,
                segment.end_sample,
                request.sample_rate
            },
            {timeline_cursor, timeline_cursor + length}
        };
        expected_source_start = segment.end_sample;
        timeline_cursor += length;
    }
    if (expected_source_start != request.source_frames) {
        return {
            WholeFileSegmentPlanDisposition::IncompleteSourceCoverage,
            0};
    }
    return {
        WholeFileSegmentPlanDisposition::Planned,
        segments.size()};
}

} // namespace kivo::core::playback
