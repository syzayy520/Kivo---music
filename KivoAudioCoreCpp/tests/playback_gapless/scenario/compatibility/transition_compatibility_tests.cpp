#include "../../fixture/gapless_test_assert.hpp"
#include "../../fixture/gapless_test_factory.hpp"
#include "../../runner/playback_gapless_test_cases.hpp"

#include "kivo/core/playback/gapless/transition/gapless_transition_evaluator.hpp"
#include "kivo/core/playback/gapless/plan/gapless_track_plan_builder.hpp"

namespace playback_gapless_test {

void compatible_tracks_share_exact_boundary() {
    using namespace kivo::core::playback;
    const auto decision = evaluate_gapless_transition(
        plan(0, 48000, 0),
        plan(1, 96000, 48000, 48000, true));
    GAPLESS_ASSERT(decision.compatible);
    GAPLESS_ASSERT(
        decision.rejection == GaplessTransitionRejectionReason::None);
    GAPLESS_ASSERT(decision.boundary == 48000);
}

void transition_rejections_are_explicit() {
    using namespace kivo::core::playback;
    const auto current = plan(0, 100, 0);
    GAPLESS_ASSERT(
        evaluate_gapless_transition(current, plan(1, 100, 101)).rejection
        == GaplessTransitionRejectionReason::TimelineGap);
    GAPLESS_ASSERT(
        evaluate_gapless_transition(current, plan(1, 100, 99)).rejection
        == GaplessTransitionRejectionReason::TimelineOverlap);
    GAPLESS_ASSERT(
        evaluate_gapless_transition(
            current,
            plan(1, 100, 100, 44100)).rejection
        == GaplessTransitionRejectionReason::FormatMismatch);
}

void media_probe_builds_trim_aware_track_plan() {
    using namespace kivo;
    core::decode::MediaProbe probe{};
    probe.source_identity = {9};
    probe.codec = core::decode::AudioCodec::Aac;
    probe.container = core::decode::MediaContainer::Mp4;
    probe.native_format = {format().format};
    probe.output_format = format();
    probe.resample = {
        false,
        core::contract::ResampleReason::NotNeeded,
        48000,
        48000
    };
    probe.codec_delay = {1024};
    probe.trailing_padding = {512};
    probe.duration_frames = 96000;
    probe.duration_known = true;
    GAPLESS_ASSERT(probe.is_valid());

    const auto track = core::playback::build_gapless_track_plan(
        probe,
        3,
        48000,
        true,
        {2048},
        {4096});
    GAPLESS_ASSERT(track);
    GAPLESS_ASSERT(track->audible_frames == 96000);
    GAPLESS_ASSERT(track->timeline_end() == 144000);
    GAPLESS_ASSERT(track->metadata.codec_delay.samples == 1024);
    GAPLESS_ASSERT(track->metadata.encoder_padding.samples == 512);
    GAPLESS_ASSERT(track->metadata.preroll.samples == 2048);
    GAPLESS_ASSERT(track->metadata.prefetch.samples == 4096);
}

} // namespace playback_gapless_test
