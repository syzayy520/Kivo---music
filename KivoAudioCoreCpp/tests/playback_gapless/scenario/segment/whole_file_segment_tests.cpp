#include "../../fixture/gapless_test_assert.hpp"
#include "../../runner/playback_gapless_test_cases.hpp"

#include <array>

#include "kivo/core/playback/gapless/segment/whole_file_segment_plan.hpp"

namespace playback_gapless_test {

void whole_file_segments_map_exactly() {
    using namespace kivo;
    const std::array<core::contract::VirtualTrackSegment, 3> segments{{
        {0, 0, 100, 3},
        {1, 100, 250, 3},
        {2, 250, 400, 3}
    }};
    std::array<core::contract::TrackSegmentRange, 3> output{};
    const auto result = core::playback::plan_whole_file_segments(
        {7, 48000, 400, 1000},
        segments,
        output);
    GAPLESS_ASSERT(
        result.disposition
        == core::playback::WholeFileSegmentPlanDisposition::Planned);
    GAPLESS_ASSERT(result.planned_segments == 3);
    GAPLESS_ASSERT(output[0].timeline_range.start_sample == 1000);
    GAPLESS_ASSERT(output[1].timeline_range.start_sample == 1100);
    GAPLESS_ASSERT(output[2].timeline_range.end_sample == 1400);
    GAPLESS_ASSERT(output[2].source_range.end_sample == 400);
}

void malformed_whole_file_segments_are_rejected() {
    using namespace kivo;
    const std::array<core::contract::VirtualTrackSegment, 2> gap{{
        {0, 0, 100, 2},
        {1, 101, 200, 2}
    }};
    const std::array<core::contract::VirtualTrackSegment, 2> short_cover{{
        {0, 0, 100, 2},
        {1, 100, 199, 2}
    }};
    std::array<core::contract::TrackSegmentRange, 2> output{};
    GAPLESS_ASSERT(
        core::playback::plan_whole_file_segments(
            {7, 48000, 200, 0},
            gap,
            output).disposition
        == core::playback::WholeFileSegmentPlanDisposition::
            NonContiguousSource);
    GAPLESS_ASSERT(
        core::playback::plan_whole_file_segments(
            {7, 48000, 200, 0},
            short_cover,
            output).disposition
        == core::playback::WholeFileSegmentPlanDisposition::
            IncompleteSourceCoverage);
}

} // namespace playback_gapless_test
