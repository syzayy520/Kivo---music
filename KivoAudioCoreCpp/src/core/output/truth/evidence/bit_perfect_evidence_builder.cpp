#include "kivo/core/output/truth/evidence/bit_perfect_evidence_builder.hpp"

namespace kivo::core::output {

BitPerfectEvidence build_bit_perfect_evidence(
    render::RenderOutputMode requested_mode,
    render::RenderOutputMode actual_mode,
    const contract::AudioFormatDescriptor& source_format,
    const contract::AudioFormatDescriptor& render_format,
    const contract::AudioFormatDescriptor& device_format,
    bool host_audio_engine_in_path,
    bool policy_allows_bit_perfect,
    const processing::AudioConversionSnapshot& conversion,
    const processing::AudioProcessingSnapshot& processing) noexcept {
    BitPerfectEvidence evidence{};
    evidence.requested_mode = requested_mode;
    evidence.actual_mode = actual_mode;
    evidence.source_format = source_format;
    evidence.render_format = render_format;
    evidence.device_format = device_format;
    evidence.host_audio_engine_in_path = host_audio_engine_in_path;
    evidence.resampler_active =
        conversion.conversion.sample_rate_convert;
    evidence.sample_format_conversion_active =
        conversion.conversion.sample_format_convert;
    evidence.bit_depth_conversion_active =
        conversion.conversion.bit_depth_convert;
    evidence.channel_remix_active =
        conversion.conversion.channel_remix
        || processing.participation.channel_remix_active;
    evidence.replay_gain_active =
        processing.participation.replay_gain_active;
    evidence.volume_active =
        processing.participation.volume_active;
    evidence.dsp_active = processing.participation.dsp_active;
    evidence.dither_active =
        conversion.dither_active || processing.dither_active;
    evidence.policy_allows_bit_perfect = policy_allows_bit_perfect;
    return evidence;
}

} // namespace kivo::core::output
