// =============================================================================
// KivoAudioCoreCpp — cue_boundary_tests.cpp
// Contract Tests: CUE boundary types
// =============================================================================

#include "../../contract_tests_main.h"
#include "kivo/core/contract/cue/boundary/cue_sheet_boundary.hpp"
#include "kivo/core/contract/cue/boundary/whole_album_file.hpp"
#include "kivo/core/contract/cue/boundary/virtual_track_segment.hpp"

namespace {

// --- CueSheetBoundary ---

void cue_sheet_boundary_default_construction() {
    kivo::core::contract::CueSheetBoundary b;
    ASSERT(b.track_index == 0);
    ASSERT(b.start_offset == 0);
    ASSERT(b.end_offset == 0);
    ASSERT(b.pregap_samples == 0);
}

void cue_sheet_boundary_field_modification() {
    kivo::core::contract::CueSheetBoundary b;
    b.track_index = 3;
    b.start_offset = 1000;
    b.end_offset = 5000;
    b.pregap_samples = 150;
    ASSERT(b.track_index == 3);
    ASSERT(b.start_offset == 1000);
    ASSERT(b.end_offset == 5000);
    ASSERT(b.pregap_samples == 150);
}

void cue_sheet_boundary_equality() {
    kivo::core::contract::CueSheetBoundary a{1, 100, 200, 50};
    kivo::core::contract::CueSheetBoundary b{1, 100, 200, 50};
    ASSERT(a == b);
}

void cue_sheet_boundary_inequality() {
    kivo::core::contract::CueSheetBoundary a{1, 100, 200, 50};
    kivo::core::contract::CueSheetBoundary b{2, 100, 200, 50};
    ASSERT(!(a == b));
}

// --- WholeAlbumFile ---

void whole_album_file_default_construction() {
    kivo::core::contract::WholeAlbumFile f;
    ASSERT(f.total_samples == 0);
    ASSERT(f.track_count == 0);
    ASSERT(f.is_single_file == false);
}

void whole_album_file_field_modification() {
    kivo::core::contract::WholeAlbumFile f;
    f.total_samples = 441000;
    f.track_count = 12;
    f.is_single_file = true;
    ASSERT(f.total_samples == 441000);
    ASSERT(f.track_count == 12);
    ASSERT(f.is_single_file == true);
}

void whole_album_file_equality() {
    kivo::core::contract::WholeAlbumFile a{441000, 12, true};
    kivo::core::contract::WholeAlbumFile b{441000, 12, true};
    ASSERT(a == b);
}

void whole_album_file_inequality() {
    kivo::core::contract::WholeAlbumFile a{441000, 12, true};
    kivo::core::contract::WholeAlbumFile b{441000, 10, true};
    ASSERT(!(a == b));
}

// --- VirtualTrackSegment ---

void virtual_track_segment_default_construction() {
    kivo::core::contract::VirtualTrackSegment s;
    ASSERT(s.segment_index == 0);
    ASSERT(s.start_sample == 0);
    ASSERT(s.end_sample == 0);
    ASSERT(s.parent_track_count == 0);
}

void virtual_track_segment_field_modification() {
    kivo::core::contract::VirtualTrackSegment s;
    s.segment_index = 2;
    s.start_sample = 5000;
    s.end_sample = 10000;
    s.parent_track_count = 5;
    ASSERT(s.segment_index == 2);
    ASSERT(s.start_sample == 5000);
    ASSERT(s.end_sample == 10000);
    ASSERT(s.parent_track_count == 5);
}

void virtual_track_segment_equality() {
    kivo::core::contract::VirtualTrackSegment a{2, 5000, 10000, 5};
    kivo::core::contract::VirtualTrackSegment b{2, 5000, 10000, 5};
    ASSERT(a == b);
}

void virtual_track_segment_inequality() {
    kivo::core::contract::VirtualTrackSegment a{2, 5000, 10000, 5};
    kivo::core::contract::VirtualTrackSegment b{3, 5000, 10000, 5};
    ASSERT(!(a == b));
}

} // namespace

void run_cue_boundary_tests(ContractTestRunner& runner) {
    runner.run("cue_sheet_boundary_default_construction", cue_sheet_boundary_default_construction);
    runner.run("cue_sheet_boundary_field_modification", cue_sheet_boundary_field_modification);
    runner.run("cue_sheet_boundary_equality", cue_sheet_boundary_equality);
    runner.run("cue_sheet_boundary_inequality", cue_sheet_boundary_inequality);
    runner.run("whole_album_file_default_construction", whole_album_file_default_construction);
    runner.run("whole_album_file_field_modification", whole_album_file_field_modification);
    runner.run("whole_album_file_equality", whole_album_file_equality);
    runner.run("whole_album_file_inequality", whole_album_file_inequality);
    runner.run("virtual_track_segment_default_construction", virtual_track_segment_default_construction);
    runner.run("virtual_track_segment_field_modification", virtual_track_segment_field_modification);
    runner.run("virtual_track_segment_equality", virtual_track_segment_equality);
    runner.run("virtual_track_segment_inequality", virtual_track_segment_inequality);
}
