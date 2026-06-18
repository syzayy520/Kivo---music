#include "host/abi/engine/host_audio_engine.hpp"

#include <algorithm>
#include <cstring>
#include <optional>

#include "kivo/core/contract/format/descriptor/audio_format_descriptor.hpp"
#include "kivo/core/contract/output/truth/bitperfect_rejection_reason.hpp"
#include "kivo/core/contract/output/truth/bitperfect_truth_report.hpp"
#include "kivo/core/output/truth/evidence/bit_perfect_evidence.hpp"
#include "kivo/core/render/format/render_output_mode.hpp"

namespace kivo::host::abi {

namespace {

// The flat ABI rejection-category enum MUST equal the core enum numerically.
// These static_asserts freeze that contract: a reorder of either enum fails the
// build instead of silently shipping a wrong wire value.
using CoreCat = core::contract::BitPerfectRejectionCategory;
static_assert(static_cast<uint32_t>(CoreCat::Unknown)
    == KIVO_AUDIO_BP_REJECT_UNKNOWN);
static_assert(static_cast<uint32_t>(CoreCat::FormatMismatch)
    == KIVO_AUDIO_BP_REJECT_FORMAT_MISMATCH);
static_assert(static_cast<uint32_t>(CoreCat::SampleRateMismatch)
    == KIVO_AUDIO_BP_REJECT_SAMPLE_RATE_MISMATCH);
static_assert(static_cast<uint32_t>(CoreCat::ChannelLayoutMismatch)
    == KIVO_AUDIO_BP_REJECT_CHANNEL_LAYOUT_MISMATCH);
static_assert(static_cast<uint32_t>(CoreCat::ProcessingActive)
    == KIVO_AUDIO_BP_REJECT_PROCESSING_ACTIVE);
static_assert(static_cast<uint32_t>(CoreCat::EngineParticipation)
    == KIVO_AUDIO_BP_REJECT_ENGINE_PARTICIPATION);
static_assert(static_cast<uint32_t>(CoreCat::PolicyForbidden)
    == KIVO_AUDIO_BP_REJECT_POLICY_FORBIDDEN);
static_assert(static_cast<uint32_t>(CoreCat::EvidenceIncomplete)
    == KIVO_AUDIO_BP_REJECT_EVIDENCE_INCOMPLETE);
static_assert(static_cast<uint32_t>(CoreCat::SharedMode)
    == KIVO_AUDIO_BP_REJECT_SHARED_MODE);
static_assert(static_cast<uint32_t>(CoreCat::ModeMismatch)
    == KIVO_AUDIO_BP_REJECT_MODE_MISMATCH);
static_assert(static_cast<uint32_t>(CoreCat::ResamplingActive)
    == KIVO_AUDIO_BP_REJECT_RESAMPLING_ACTIVE);
static_assert(static_cast<uint32_t>(CoreCat::ConversionActive)
    == KIVO_AUDIO_BP_REJECT_CONVERSION_ACTIVE);
static_assert(static_cast<uint32_t>(CoreCat::ReplayGainActive)
    == KIVO_AUDIO_BP_REJECT_REPLAY_GAIN_ACTIVE);
static_assert(static_cast<uint32_t>(CoreCat::VolumeActive)
    == KIVO_AUDIO_BP_REJECT_VOLUME_ACTIVE);
static_assert(static_cast<uint32_t>(CoreCat::DspActive)
    == KIVO_AUDIO_BP_REJECT_DSP_ACTIVE);
static_assert(static_cast<uint32_t>(CoreCat::DitherActive)
    == KIVO_AUDIO_BP_REJECT_DITHER_ACTIVE);

void set_bool_bit(
    uint64_t& known,
    uint64_t& values,
    kivo_audio_truth_bool_bit bit,
    const std::optional<bool>& field) noexcept {
    if (!field.has_value()) {
        return;
    }
    const uint64_t mask = static_cast<uint64_t>(1) << static_cast<unsigned>(bit);
    known |= mask;
    if (*field) {
        values |= mask;
    }
}

uint32_t map_mode(
    const std::optional<core::render::RenderOutputMode>& mode) noexcept {
    if (!mode.has_value()) {
        return KIVO_AUDIO_TRUTH_MODE_UNKNOWN;
    }
    return *mode == core::render::RenderOutputMode::Exclusive
        ? KIVO_AUDIO_TRUTH_MODE_EXCLUSIVE
        : KIVO_AUDIO_TRUTH_MODE_SHARED;
}

kivo_audio_format_descriptor map_format(
    const std::optional<core::contract::AudioFormatDescriptor>& format) noexcept {
    kivo_audio_format_descriptor out{};
    if (!format.has_value()) {
        return out;  // all-zero => sample_rate 0 => unknown
    }
    out.sample_rate = format->sample_rate;
    out.channel_mask =
        static_cast<uint32_t>(format->effective_channel_mask().value);
    out.channel_count = static_cast<uint16_t>(format->channel_count());
    out.sample_format = static_cast<uint8_t>(format->sample_format);
    out.bits_per_sample = format->bits_per_sample;
    out.container_bits = format->container_bits();
    out.frame_layout = static_cast<uint8_t>(format->frame_layout);
    return out;
}

uint32_t map_verdict(
    const core::contract::BitPerfectTruthReport& report,
    const core::output::BitPerfectEvidence& evidence) noexcept {
    if (report.is_bit_perfect()) {
        return KIVO_AUDIO_BIT_PERFECT_VERDICT_BIT_PERFECT;
    }
    if (!evidence.is_complete()) {
        return KIVO_AUDIO_BIT_PERFECT_VERDICT_EVIDENCE_INCOMPLETE;
    }
    if (report.was_rejected()) {
        return KIVO_AUDIO_BIT_PERFECT_VERDICT_REJECTED;
    }
    return KIVO_AUDIO_BIT_PERFECT_VERDICT_UNKNOWN;
}

} // namespace

kivo_audio_result HostAudioEngine::truth(
    void* truth,
    uint32_t truth_size) noexcept {
    if (truth == nullptr || truth_size < KIVO_AUDIO_TRUTH_V1_BASE_SIZE) {
        return KIVO_AUDIO_RESULT_INVALID_STRUCT;
    }
    std::scoped_lock lock{command_mutex_};
    const auto current = runtime_.snapshot();  // builds + evaluates under coord mutex
    const auto& evidence = current.bit_perfect_evidence;
    const auto& report = current.bit_perfect_report;

    kivo_audio_truth_v1 result{};
    result.struct_size = sizeof(kivo_audio_truth_v1);
    result.struct_version = KIVO_AUDIO_ABI_STRUCT_VERSION_1;

    set_bool_bit(result.known_mask, result.bool_mask,
        KIVO_AUDIO_TRUTH_BOOL_HOST_ENGINE_IN_PATH,
        evidence.host_audio_engine_in_path);
    set_bool_bit(result.known_mask, result.bool_mask,
        KIVO_AUDIO_TRUTH_BOOL_RESAMPLER_ACTIVE, evidence.resampler_active);
    set_bool_bit(result.known_mask, result.bool_mask,
        KIVO_AUDIO_TRUTH_BOOL_SAMPLE_FORMAT_CONVERSION,
        evidence.sample_format_conversion_active);
    set_bool_bit(result.known_mask, result.bool_mask,
        KIVO_AUDIO_TRUTH_BOOL_BIT_DEPTH_CONVERSION,
        evidence.bit_depth_conversion_active);
    set_bool_bit(result.known_mask, result.bool_mask,
        KIVO_AUDIO_TRUTH_BOOL_CHANNEL_REMIX, evidence.channel_remix_active);
    set_bool_bit(result.known_mask, result.bool_mask,
        KIVO_AUDIO_TRUTH_BOOL_REPLAY_GAIN_ACTIVE, evidence.replay_gain_active);
    set_bool_bit(result.known_mask, result.bool_mask,
        KIVO_AUDIO_TRUTH_BOOL_VOLUME_ACTIVE, evidence.volume_active);
    set_bool_bit(result.known_mask, result.bool_mask,
        KIVO_AUDIO_TRUTH_BOOL_DSP_ACTIVE, evidence.dsp_active);
    set_bool_bit(result.known_mask, result.bool_mask,
        KIVO_AUDIO_TRUTH_BOOL_DITHER_ACTIVE, evidence.dither_active);
    set_bool_bit(result.known_mask, result.bool_mask,
        KIVO_AUDIO_TRUTH_BOOL_POLICY_ALLOWS_BIT_PERFECT,
        evidence.policy_allows_bit_perfect);

    result.verdict = map_verdict(report, evidence);
    result.rejection_category =
        static_cast<uint32_t>(report.rejection_reason.category);
    result.evidence_complete = report.evidence_complete ? 1u : 0u;
    result.requested_mode = map_mode(evidence.requested_mode);
    result.actual_mode = map_mode(evidence.actual_mode);
    result.source_format = map_format(evidence.source_format);
    result.render_format = map_format(evidence.render_format);
    result.device_format = map_format(evidence.device_format);

    std::memcpy(
        truth,
        &result,
        std::min<std::size_t>(truth_size, sizeof(result)));
    return KIVO_AUDIO_RESULT_OK;
}

} // namespace kivo::host::abi
