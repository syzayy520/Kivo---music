// =============================================================================
// KivoAudioCoreCpp — cue_range_tests.cpp
// Contract Tests: CUE range types
// =============================================================================

#include "../../contract_tests_main.h"
#include "kivo/core/contract/cue/range/source_time_range.hpp"
#include "kivo/core/contract/cue/range/timeline_range.hpp"
#include "kivo/core/contract/cue/range/track_segment_range.hpp"

namespace {

// --- SourceTimeRange ---

void source_time_range_default_construction() {
    kivo::core::contract::SourceTimeRange r;
    ASSERT(r.source_ref == 0);
    ASSERT(r.start_sample == 0);
    ASSERT(r.end_sample == 0);
    ASSERT(r.sample_rate == 0);
}

void source_time_range_field_modification() {
    kivo::core::contract::SourceTimeRange r;
    r.source_ref = 42;
    r.start_sample = 1000;
    r.end_sample = 5000;
    r.sample_rate = 44100;
    ASSERT(r.source_ref == 42);
    ASSERT(r.start_sample == 1000);
    ASSERT(r.end_sample == 5000);
    ASSERT(r.sample_rate == 44100);
}

void source_time_range_equality() {
    kivo::core::contract::SourceTimeRange a{42, 1000, 5000, 44100};
    kivo::core::contract::SourceTimeRange b{42, 1000, 5000, 44100};
    ASSERT(a == b);
}

void source_time_range_inequality() {
    kivo::core::contract::SourceTimeRange a{42, 1000, 5000, 44100};
    kivo::core::contract::SourceTimeRange b{43, 1000, 5000, 44100};
    ASSERT(!(a == b));
}

// --- TimelineRange ---

void timeline_range_default_construction() {
    kivo::core::contract::TimelineRange r;
    ASSERT(r.start_sample == 0);
    ASSERT(r.end_sample == 0);
}

void timeline_range_field_modification() {
    kivo::core::contract::TimelineRange r;
    r.start_sample = 2000;
    r.end_sample = 8000;
    ASSERT(r.start_sample == 2000);
    ASSERT(r.end_sample == 8000);
}

void timeline_range_equality() {
    kivo::core::contract::TimelineRange a{2000, 8000};
    kivo::core::contract::TimelineRange b{2000, 8000};
    ASSERT(a == b);
}

void timeline_range_inequality() {
    kivo::core::contract::TimelineRange a{2000, 8000};
    kivo::core::contract::TimelineRange b{2000, 9000};
    ASSERT(!(a == b));
}

// --- TrackSegmentRange ---

void track_segment_range_default_construction() {
    kivo::core::contract::TrackSegmentRange r;
    ASSERT(r.segment_index == 0);
    ASSERT(r.source_range.source_ref == 0);
    ASSERT(r.timeline_range.start_sample == 0);
}

void track_segment_range_field_modification() {
    kivo::core::contract::TrackSegmentRange r;
    r.segment_index = 1;
    r.source_range = {42, 1000, 5000, 44100};
    r.timeline_range = {2000, 8000};
    ASSERT(r.segment_index == 1);
    ASSERT(r.source_range.source_ref == 42);
    ASSERT(r.timeline_range.start_sample == 2000);
}

void track_segment_range_equality() {
    kivo::core::contract::SourceTimeRange sr{42, 1000, 5000, 44100};
    kivo::core::contract::TimelineRange tr{2000, 8000};
    kivo::core::contract::TrackSegmentRange a{1, sr, tr};
    kivo::core::contract::TrackSegmentRange b{1, sr, tr};
    ASSERT(a == b);
}

void track_segment_range_inequality() {
    kivo::core::contract::SourceTimeRange sr{42, 1000, 5000, 44100};
    kivo::core::contract::TimelineRange tr{2000, 8000};
    kivo::core::contract::TrackSegmentRange a{1, sr, tr};
    kivo::core::contract::TrackSegmentRange b{2, sr, tr};
    ASSERT(!(a == b));
}

} // namespace

void run_cue_range_tests(ContractTestRunner& runner) {
    runner.run("source_time_range_default_construction", source_time_range_default_construction);
    runner.run("source_time_range_field_modification", source_time_range_field_modification);
    runner.run("source_time_range_equality", source_time_range_equality);
    runner.run("source_time_range_inequality", source_time_range_inequality);
    runner.run("timeline_range_default_construction", timeline_range_default_construction);
    runner.run("timeline_range_field_modification", timeline_range_field_modification);
    runner.run("timeline_range_equality", timeline_range_equality);
    runner.run("timeline_range_inequality", timeline_range_inequality);
    runner.run("track_segment_range_default_construction", track_segment_range_default_construction);
    runner.run("track_segment_range_field_modification", track_segment_range_field_modification);
    runner.run("track_segment_range_equality", track_segment_range_equality);
    runner.run("track_segment_range_inequality", track_segment_range_inequality);
}
