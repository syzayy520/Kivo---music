#include "host/abi/engine/host_audio_engine.hpp"

#include <cmath>
#include <limits>
#include <optional>

#include "host/abi/mapping/host_result_mapping.hpp"
#include "host/abi/validation/host_abi_validation.hpp"

namespace kivo::host::abi {

namespace {

[[nodiscard]] core::contract::SampleFormat sample_format(
    uint32_t value) noexcept {
    using Format = core::contract::SampleFormat;
    switch (value) {
    case KIVO_AUDIO_SAMPLE_FORMAT_INT16: return Format::Int16;
    case KIVO_AUDIO_SAMPLE_FORMAT_INT24_IN_32: return Format::Int24;
    case KIVO_AUDIO_SAMPLE_FORMAT_INT32: return Format::Int32;
    case KIVO_AUDIO_SAMPLE_FORMAT_FLOAT32: return Format::Float32;
    case KIVO_AUDIO_SAMPLE_FORMAT_FLOAT64: return Format::Float64;
    default: return Format::Unknown;
    }
}

[[nodiscard]] core::contract::ChannelLayout channel_layout(
    uint32_t value) noexcept {
    using Layout = core::contract::ChannelLayout;
    switch (value) {
    case KIVO_AUDIO_CHANNEL_LAYOUT_MONO: return Layout::Mono;
    case KIVO_AUDIO_CHANNEL_LAYOUT_STEREO: return Layout::Stereo;
    case KIVO_AUDIO_CHANNEL_LAYOUT_SURROUND_5_1: return Layout::Surround51;
    case KIVO_AUDIO_CHANNEL_LAYOUT_SURROUND_7_1: return Layout::Surround71;
    default: return Layout::Unknown;
    }
}

[[nodiscard]] core::processing::ResampleQuality resample_quality(
    uint32_t value) noexcept {
    using Quality = core::processing::ResampleQuality;
    switch (value) {
    case KIVO_AUDIO_RESAMPLE_QUALITY_FAST: return Quality::Fast;
    case KIVO_AUDIO_RESAMPLE_QUALITY_HIGH: return Quality::High;
    default: return Quality::Balanced;
    }
}

[[nodiscard]] core::processing::DitherPolicy dither_policy(
    uint32_t value) noexcept {
    return value == KIVO_AUDIO_DITHER_TRIANGULAR_PDF
        ? core::processing::DitherPolicy::TriangularPdf
        : core::processing::DitherPolicy::Disabled;
}

[[nodiscard]] bool valid_configuration(
    const kivo_audio_open_configuration_v1& configuration) noexcept {
    return sample_format(configuration.sample_format)
            != core::contract::SampleFormat::Unknown
        && channel_layout(configuration.channel_layout)
            != core::contract::ChannelLayout::Unknown
        && configuration.sample_rate != 0u
        && configuration.requested_buffer_frames != 0u
        && configuration.queue_slot_count
            >= core::render::SpscAudioBlockQueueConfiguration::kMinimumSlotCount
        && configuration.queue_slot_count
            <= core::render::SpscAudioBlockQueueConfiguration::kMaximumSlotCount
        && configuration.maximum_chunk_frames != 0u
        && configuration.output_mode
            <= KIVO_AUDIO_OUTPUT_MODE_EXCLUSIVE
        && configuration.resample_quality
            <= KIVO_AUDIO_RESAMPLE_QUALITY_HIGH
        && configuration.conversion_dither
            <= KIVO_AUDIO_DITHER_TRIANGULAR_PDF
        && configuration.processing_dither
            <= KIVO_AUDIO_DITHER_TRIANGULAR_PDF
        && is_boolean(configuration.allow_conversion)
        && is_boolean(configuration.bit_perfect_required)
        && is_boolean(configuration.replay_gain_enabled)
        && is_boolean(configuration.replay_gain_peak_known)
        && is_boolean(configuration.prevent_clipping)
        && is_boolean(configuration.software_volume_enabled)
        && std::isfinite(configuration.replay_gain_db)
        && std::isfinite(configuration.replay_gain_peak)
        && std::isfinite(configuration.software_volume_linear_gain)
        && configuration.dither_seed != 0u;
}

[[nodiscard]] core::playback::PlaybackRuntimeOpenRequest
make_open_request(
    const kivo_audio_open_configuration_v1& configuration,
    core::contract::DeviceGeneration device_generation,
    uint64_t source_generation) noexcept {
    const auto format = sample_format(configuration.sample_format);
    core::playback::PlaybackRuntimeOpenRequest request{};
    request.render_request.requested_format = {{
        format,
        channel_layout(configuration.channel_layout),
        core::contract::FrameLayout::Interleaved,
        {},
        configuration.sample_rate,
        core::contract::sample_format_valid_bits(format),
        core::contract::sample_format_container_bits(format)
    }};
    request.render_request.requested_buffer_frames =
        configuration.requested_buffer_frames;
    request.render_request.output_mode =
        configuration.output_mode == KIVO_AUDIO_OUTPUT_MODE_EXCLUSIVE
            ? core::render::RenderOutputMode::Exclusive
            : core::render::RenderOutputMode::Shared;
    request.render_request.format_policy =
        configuration.allow_conversion != 0u
            ? core::render::FormatAcceptancePolicy::AllowCompatible
            : core::render::FormatAcceptancePolicy::Exact;
    request.render_request.generations.device = device_generation;

    const auto bytes_per_frame =
        request.render_request.requested_format.format.bytes_per_frame();
    const auto slot_bytes =
        static_cast<size_t>(configuration.maximum_chunk_frames)
        * bytes_per_frame;
    request.queue_configuration = {
        configuration.queue_slot_count,
        slot_bytes
    };
    request.producer_configuration.maximum_decode_block_bytes =
        static_cast<size_t>(configuration.sample_rate)
        * bytes_per_frame;
    request.producer_configuration.maximum_chunk_frames =
        configuration.maximum_chunk_frames;
    request.producer_configuration.processing.replay_gain.enabled =
        configuration.replay_gain_enabled != 0u;
    request.producer_configuration.processing.replay_gain.gain_db =
        configuration.replay_gain_db;
    if (configuration.replay_gain_peak_known != 0u) {
        request.producer_configuration.processing.replay_gain.peak =
            configuration.replay_gain_peak;
    }
    request.producer_configuration.processing.replay_gain.prevent_clipping =
        configuration.prevent_clipping != 0u;
    request.producer_configuration.processing.volume.enabled =
        configuration.software_volume_enabled != 0u;
    request.producer_configuration.processing.volume.linear_gain =
        configuration.software_volume_linear_gain;
    request.producer_configuration.processing.dither_policy =
        dither_policy(configuration.processing_dither);
    request.producer_configuration.processing.dither_seed =
        configuration.dither_seed;
    request.producer_configuration.processing.bit_perfect_required =
        configuration.bit_perfect_required != 0u;
    request.decode_generation = {
        core::contract::GenerationId::from_value(source_generation)
    };
    request.allow_conversion = configuration.allow_conversion != 0u;
    request.resample_quality =
        resample_quality(configuration.resample_quality);
    request.conversion_dither =
        dither_policy(configuration.conversion_dither);
    return request;
}

} // namespace

kivo_audio_result HostAudioEngine::open_source(
    uint64_t command_id,
    uint64_t session_generation,
    std::unique_ptr<core::decode::ByteSourceBoundary> source,
    const kivo_audio_open_configuration_v1& configuration) noexcept {
    std::unique_lock lock{command_mutex_, std::try_to_lock};
    if (!lock.owns_lock()) {
        return KIVO_AUDIO_RESULT_BUSY;
    }
    if (command_id == 0u || session_generation == 0u
        || !source || !valid_configuration(configuration)) {
        last_result_ = KIVO_AUDIO_RESULT_INVALID_ARGUMENT;
        return last_result_;
    }
    const auto request = make_open_request(
        configuration,
        renderer_.capabilities().device_generation,
        source->generation().value());
    if (!request.is_valid()) {
        last_result_ = KIVO_AUDIO_RESULT_INVALID_ARGUMENT;
        return last_result_;
    }
    const core::playback::PlaybackCommand command{
        {command_id},
        core::contract::CommandKind::OpenSource,
        {
            session_generation,
            core::contract::CommandOrigin::External
        },
        core::contract::kInvalidSamplePosition
    };
    const auto runtime_result =
        runtime_.open(command, std::move(source), request);
    const auto result = map_runtime_result(runtime_result);
    last_result_ = result;
    lock.unlock();
    if (result != KIVO_AUDIO_RESULT_OK) {
        emit(
            result,
            KIVO_AUDIO_DIAGNOSTIC_DOMAIN_LIFECYCLE,
            command_id);
    }
    return result;
}

} // namespace kivo::host::abi
