#pragma once

#include "kivo/core/output/truth/evidence/bit_perfect_evidence.hpp"

namespace output_truth_test {

[[nodiscard]] constexpr kivo::core::contract::AudioFormatDescriptor
format(uint32_t sample_rate = 48000) noexcept {
    using namespace kivo::core::contract;
    return {
        SampleFormat::Int24,
        ChannelLayout::Stereo,
        FrameLayout::Interleaved,
        {},
        sample_rate,
        24,
        32
    };
}

[[nodiscard]] inline kivo::core::output::BitPerfectEvidence
clean_evidence() {
    using namespace kivo::core;
    output::BitPerfectEvidence evidence{};
    evidence.requested_mode = render::RenderOutputMode::Exclusive;
    evidence.actual_mode = render::RenderOutputMode::Exclusive;
    evidence.source_format = format();
    evidence.render_format = format();
    evidence.device_format = format();
    evidence.host_audio_engine_in_path = false;
    evidence.resampler_active = false;
    evidence.sample_format_conversion_active = false;
    evidence.bit_depth_conversion_active = false;
    evidence.channel_remix_active = false;
    evidence.replay_gain_active = false;
    evidence.volume_active = false;
    evidence.dsp_active = false;
    evidence.dither_active = false;
    evidence.policy_allows_bit_perfect = true;
    return evidence;
}

} // namespace output_truth_test
