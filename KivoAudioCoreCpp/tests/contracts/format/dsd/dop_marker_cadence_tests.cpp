#include "kivo/core/contract/format/dsd/dop_marker_cadence.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void default_dop_marker_cadence_alternates_per_pcm_frame() {
    ASSERT(expected_dop_marker_for_frame(0) == 0x05);
    ASSERT(expected_dop_marker_for_frame(1) == 0xFA);
    ASSERT(expected_dop_marker_for_frame(2) == 0x05);
}

static void dop_marker_cadence_accepts_reversed_phase() {
    const DopMarkerPair reversed{0xFA, 0x05};
    ASSERT(expected_dop_marker_for_frame(0, reversed) == 0xFA);
    ASSERT(dop_marker_matches_frame(1, 0x05, reversed));
}

static void dop_marker_cadence_rejects_wrong_marker_for_frame() {
    ASSERT(!dop_marker_matches_frame(0, 0xFA));
    ASSERT(!dop_marker_matches_frame(1, 0x05));
}

static void dop_marker_cadence_rejects_invalid_marker_pair() {
    const DopMarkerPair invalid{0x05, 0x00};
    ASSERT(!dop_marker_matches_frame(0, 0x05, invalid));
}

static void dop_frame_marker_sync_requires_channel_alignment() {
    const DopFrameMarkerSync frame{0x05, 0x05, 2};
    ASSERT(frame.channels_are_synchronized());
    ASSERT(frame.matches_frame(0));
}

static void dop_frame_marker_sync_rejects_cross_channel_phase_mismatch() {
    const DopFrameMarkerSync frame{0x05, 0xFA, 2};
    ASSERT(!frame.channels_are_synchronized());
    ASSERT(!frame.matches_frame(0));
}

static void dop_frame_marker_sync_rejects_missing_channels() {
    const DopFrameMarkerSync frame{0x05, 0x05, 0};
    ASSERT(!frame.channels_are_synchronized());
    ASSERT(!frame.matches_frame(0));
}

void run_dop_marker_cadence_tests(ContractTestRunner& runner) {
    std::cout << "--- DopMarkerCadence ---\n";
    runner.run("default_dop_marker_cadence_alternates_per_pcm_frame",
               default_dop_marker_cadence_alternates_per_pcm_frame);
    runner.run("dop_marker_cadence_accepts_reversed_phase",
               dop_marker_cadence_accepts_reversed_phase);
    runner.run("dop_marker_cadence_rejects_wrong_marker_for_frame",
               dop_marker_cadence_rejects_wrong_marker_for_frame);
    runner.run("dop_marker_cadence_rejects_invalid_marker_pair",
               dop_marker_cadence_rejects_invalid_marker_pair);
    runner.run("dop_frame_marker_sync_requires_channel_alignment",
               dop_frame_marker_sync_requires_channel_alignment);
    runner.run("dop_frame_marker_sync_rejects_cross_channel_phase_mismatch",
               dop_frame_marker_sync_rejects_cross_channel_phase_mismatch);
    runner.run("dop_frame_marker_sync_rejects_missing_channels",
               dop_frame_marker_sync_rejects_missing_channels);
    std::cout << "\n";
}
