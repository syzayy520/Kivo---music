// =============================================================================
// KivoAudioCoreCpp — cue_continuity_tests.cpp
// Contract Tests: CUE continuity types
// =============================================================================

#include "../../contract_tests_main.h"
#include "kivo/core/contract/cue/continuity/codec_delay.hpp"
#include "kivo/core/contract/cue/continuity/padding.hpp"
#include "kivo/core/contract/cue/continuity/preroll.hpp"
#include "kivo/core/contract/cue/continuity/prefetch.hpp"
#include "kivo/core/contract/cue/continuity/gapless_metadata.hpp"
#include "kivo/core/contract/cue/continuity/cross_track_clock_continuity.hpp"

namespace {

// --- CodecDelay ---

void codec_delay_default_construction() {
    kivo::core::contract::CodecDelay d;
    ASSERT(d.samples == 0);
}

void codec_delay_field_modification() {
    kivo::core::contract::CodecDelay d;
    d.samples = 2112;
    ASSERT(d.samples == 2112);
}

void codec_delay_equality() {
    kivo::core::contract::CodecDelay a{2112};
    kivo::core::contract::CodecDelay b{2112};
    ASSERT(a == b);
}

// --- Padding ---

void padding_default_construction() {
    kivo::core::contract::Padding p;
    ASSERT(p.samples == 0);
}

void padding_field_modification() {
    kivo::core::contract::Padding p;
    p.samples = 576;
    ASSERT(p.samples == 576);
}

void padding_equality() {
    kivo::core::contract::Padding a{576};
    kivo::core::contract::Padding b{576};
    ASSERT(a == b);
}

// --- Preroll ---

void preroll_default_construction() {
    kivo::core::contract::Preroll p;
    ASSERT(p.samples == 0);
}

void preroll_field_modification() {
    kivo::core::contract::Preroll p;
    p.samples = 1024;
    ASSERT(p.samples == 1024);
}

void preroll_equality() {
    kivo::core::contract::Preroll a{1024};
    kivo::core::contract::Preroll b{1024};
    ASSERT(a == b);
}

// --- Prefetch ---

void prefetch_default_construction() {
    kivo::core::contract::Prefetch p;
    ASSERT(p.samples == 0);
}

void prefetch_field_modification() {
    kivo::core::contract::Prefetch p;
    p.samples = 4096;
    ASSERT(p.samples == 4096);
}

void prefetch_equality() {
    kivo::core::contract::Prefetch a{4096};
    kivo::core::contract::Prefetch b{4096};
    ASSERT(a == b);
}

// --- GaplessMetadata ---

void gapless_metadata_default_construction() {
    kivo::core::contract::GaplessMetadata m;
    ASSERT(m.codec_delay.samples == 0);
    ASSERT(m.encoder_padding.samples == 0);
    ASSERT(m.preroll.samples == 0);
    ASSERT(m.prefetch.samples == 0);
}

void gapless_metadata_field_modification() {
    kivo::core::contract::GaplessMetadata m;
    m.codec_delay = {2112};
    m.encoder_padding = {576};
    m.preroll = {1024};
    m.prefetch = {4096};
    ASSERT(m.codec_delay.samples == 2112);
    ASSERT(m.encoder_padding.samples == 576);
    ASSERT(m.preroll.samples == 1024);
    ASSERT(m.prefetch.samples == 4096);
}

void gapless_metadata_equality() {
    kivo::core::contract::GaplessMetadata a{{2112}, {576}, {1024}, {4096}};
    kivo::core::contract::GaplessMetadata b{{2112}, {576}, {1024}, {4096}};
    ASSERT(a == b);
}

void gapless_metadata_inequality() {
    kivo::core::contract::GaplessMetadata a{{2112}, {576}, {1024}, {4096}};
    kivo::core::contract::GaplessMetadata b{{2112}, {576}, {1024}, {8192}};
    ASSERT(!(a == b));
}

// --- CrossTrackClockContinuity ---

void cross_track_clock_continuity_default_construction() {
    kivo::core::contract::CrossTrackClockContinuity c;
    ASSERT(c.previous_track_end_sample == 0);
    ASSERT(c.next_track_start_sample == 0);
    ASSERT(c.accumulated_offset_samples == 0);
    ASSERT(c.continuity_valid == false);
}

void cross_track_clock_continuity_field_modification() {
    kivo::core::contract::CrossTrackClockContinuity c;
    c.previous_track_end_sample = 441000;
    c.next_track_start_sample = 441000;
    c.accumulated_offset_samples = 2112;
    c.continuity_valid = true;
    ASSERT(c.previous_track_end_sample == 441000);
    ASSERT(c.next_track_start_sample == 441000);
    ASSERT(c.accumulated_offset_samples == 2112);
    ASSERT(c.continuity_valid == true);
}

void cross_track_clock_continuity_equality() {
    kivo::core::contract::CrossTrackClockContinuity a{441000, 441000, 2112, true};
    kivo::core::contract::CrossTrackClockContinuity b{441000, 441000, 2112, true};
    ASSERT(a == b);
}

void cross_track_clock_continuity_inequality() {
    kivo::core::contract::CrossTrackClockContinuity a{441000, 441000, 2112, true};
    kivo::core::contract::CrossTrackClockContinuity b{441000, 441000, 0, false};
    ASSERT(!(a == b));
}

} // namespace

void run_cue_continuity_tests(ContractTestRunner& runner) {
    runner.run("codec_delay_default_construction", codec_delay_default_construction);
    runner.run("codec_delay_field_modification", codec_delay_field_modification);
    runner.run("codec_delay_equality", codec_delay_equality);
    runner.run("padding_default_construction", padding_default_construction);
    runner.run("padding_field_modification", padding_field_modification);
    runner.run("padding_equality", padding_equality);
    runner.run("preroll_default_construction", preroll_default_construction);
    runner.run("preroll_field_modification", preroll_field_modification);
    runner.run("preroll_equality", preroll_equality);
    runner.run("prefetch_default_construction", prefetch_default_construction);
    runner.run("prefetch_field_modification", prefetch_field_modification);
    runner.run("prefetch_equality", prefetch_equality);
    runner.run("gapless_metadata_default_construction", gapless_metadata_default_construction);
    runner.run("gapless_metadata_field_modification", gapless_metadata_field_modification);
    runner.run("gapless_metadata_equality", gapless_metadata_equality);
    runner.run("gapless_metadata_inequality", gapless_metadata_inequality);
    runner.run("cross_track_clock_continuity_default_construction", cross_track_clock_continuity_default_construction);
    runner.run("cross_track_clock_continuity_field_modification", cross_track_clock_continuity_field_modification);
    runner.run("cross_track_clock_continuity_equality", cross_track_clock_continuity_equality);
    runner.run("cross_track_clock_continuity_inequality", cross_track_clock_continuity_inequality);
}
