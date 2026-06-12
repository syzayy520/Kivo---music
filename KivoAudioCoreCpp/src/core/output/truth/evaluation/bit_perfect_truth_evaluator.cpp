#include "kivo/core/output/truth/evaluation/bit_perfect_truth_evaluator.hpp"

namespace kivo::core::output {

namespace {

using contract::BitPerfectRejectionCategory;
using contract::BitPerfectRejectionReason;

[[nodiscard]] BitPerfectEvaluationResult rejected(
    BitPerfectRejectionReason reason,
    bool complete = true) noexcept {
    contract::BitPerfectTruthReport report{};
    report.evidence_complete = complete;
    report.rejection_reason = reason;
    return {report};
}

[[nodiscard]] BitPerfectRejectionReason reason(
    BitPerfectRejectionCategory category) noexcept {
    BitPerfectRejectionReason result{};
    result.category = category;
    return result;
}

} // namespace

BitPerfectEvaluationResult evaluate_bit_perfect_truth(
    const BitPerfectEvidence& evidence) noexcept {
    if (!evidence.is_complete()) {
        auto value = reason(BitPerfectRejectionCategory::EvidenceIncomplete);
        value.evidence_incomplete = true;
        return rejected(value, false);
    }
    if (!*evidence.policy_allows_bit_perfect) {
        auto value = reason(BitPerfectRejectionCategory::PolicyForbidden);
        value.policy_forbidden = true;
        return rejected(value);
    }
    if (*evidence.requested_mode != render::RenderOutputMode::Exclusive) {
        auto value = reason(BitPerfectRejectionCategory::SharedMode);
        value.shared_mode = true;
        return rejected(value);
    }
    if (*evidence.actual_mode != *evidence.requested_mode) {
        auto value = reason(BitPerfectRejectionCategory::ModeMismatch);
        value.mode_mismatch = true;
        return rejected(value);
    }
    if (*evidence.actual_mode != render::RenderOutputMode::Exclusive) {
        auto value = reason(BitPerfectRejectionCategory::SharedMode);
        value.shared_mode = true;
        return rejected(value);
    }

    const auto& source = *evidence.source_format;
    const auto& rendered = *evidence.render_format;
    const auto& device = *evidence.device_format;
    if (source.sample_rate != rendered.sample_rate
        || source.sample_rate != device.sample_rate) {
        auto value = reason(BitPerfectRejectionCategory::SampleRateMismatch);
        value.sample_rate_mismatch = true;
        value.format_mismatch = true;
        return rejected(value);
    }
    if (source.channel_layout != rendered.channel_layout
        || source.channel_layout != device.channel_layout
        || source.effective_channel_mask()
            != rendered.effective_channel_mask()
        || source.effective_channel_mask()
            != device.effective_channel_mask()) {
        auto value = reason(
            BitPerfectRejectionCategory::ChannelLayoutMismatch);
        value.channel_layout_mismatch = true;
        value.format_mismatch = true;
        return rejected(value);
    }
    if (source != rendered || source != device) {
        auto value = reason(BitPerfectRejectionCategory::FormatMismatch);
        value.format_mismatch = true;
        return rejected(value);
    }
    if (*evidence.host_audio_engine_in_path) {
        auto value = reason(
            BitPerfectRejectionCategory::EngineParticipation);
        value.engine_in_path = true;
        return rejected(value);
    }
    if (*evidence.resampler_active) {
        auto value = reason(BitPerfectRejectionCategory::ResamplingActive);
        value.resampling_active = true;
        return rejected(value);
    }
    if (*evidence.sample_format_conversion_active
        || *evidence.bit_depth_conversion_active) {
        auto value = reason(BitPerfectRejectionCategory::ConversionActive);
        value.conversion_active = true;
        return rejected(value);
    }
    if (*evidence.channel_remix_active) {
        auto value = reason(BitPerfectRejectionCategory::ProcessingActive);
        value.processing_active = true;
        value.channel_remix_active = true;
        return rejected(value);
    }
    if (*evidence.replay_gain_active) {
        auto value = reason(BitPerfectRejectionCategory::ReplayGainActive);
        value.processing_active = true;
        value.replay_gain_active = true;
        return rejected(value);
    }
    if (*evidence.volume_active) {
        auto value = reason(BitPerfectRejectionCategory::VolumeActive);
        value.processing_active = true;
        value.volume_active = true;
        return rejected(value);
    }
    if (*evidence.dsp_active) {
        auto value = reason(BitPerfectRejectionCategory::DspActive);
        value.processing_active = true;
        value.dsp_active = true;
        return rejected(value);
    }
    if (*evidence.dither_active) {
        auto value = reason(BitPerfectRejectionCategory::DitherActive);
        value.dither_active = true;
        return rejected(value);
    }

    contract::BitPerfectTruthReport report{};
    report.bit_perfect_achieved = true;
    report.evidence_complete = true;
    report.requested_exclusive = true;
    report.actual_exclusive = true;
    report.formats_match = true;
    report.no_processing_active = true;
    report.no_engine_in_path = true;
    report.no_sample_mutation = true;
    report.resample = contract::ResampleDecision{
        false,
        contract::ResampleReason::NotNeeded,
        source.sample_rate,
        device.sample_rate};
    return {report};
}

} // namespace kivo::core::output
