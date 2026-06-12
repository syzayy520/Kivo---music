#include "platform/windows/wasapi/renderer/wasapi_renderer_state.hpp"

#include <limits>

#include "platform/windows/wasapi/renderer/wasapi_stream_open_result.hpp"

namespace kivo::platform::windows::wasapi::detail {

namespace {

[[nodiscard]] std::chrono::nanoseconds reference_time_duration(
    REFERENCE_TIME value) noexcept {
    constexpr auto units_to_nanoseconds = int64_t{100};
    const auto maximum = std::numeric_limits<int64_t>::max();
    const auto count = value > maximum / units_to_nanoseconds
        ? maximum
        : static_cast<int64_t>(value) * units_to_nanoseconds;
    return std::chrono::nanoseconds{count};
}

[[nodiscard]] std::chrono::nanoseconds frame_duration(
    core::contract::FrameCount frames,
    uint32_t sample_rate) noexcept {
    constexpr uint64_t nanoseconds_per_second = 1'000'000'000ull;
    const auto numerator =
        static_cast<uint64_t>(frames) * nanoseconds_per_second;
    const auto count = numerator / sample_rate
        + (numerator % sample_rate == 0 ? 0 : 1);
    return std::chrono::nanoseconds{static_cast<int64_t>(count)};
}

} // namespace

core::render::RenderOpenResult WasapiRendererState::complete_open(
    const core::render::RenderOpenRequest& request,
    const WasapiStreamOpenResult& stream,
    uint64_t endpoint_identity,
    REFERENCE_TIME default_device_period,
    REFERENCE_TIME minimum_device_period) noexcept {
    if (!stream.succeeded()) {
        return fail_open(E_UNEXPECTED);
    }
    if (opened_before_) {
        device_generation_.id =
            core::contract::GenerationId::next(device_generation_.id);
    }
    opened_before_ = true;
    control_thread_id_ = GetCurrentThreadId();

    capabilities_.identity = {endpoint_identity};
    capabilities_.device_generation = device_generation_;
    capabilities_.render_capability.render_level =
        core::contract::CapabilityLevel::Standard;
    capabilities_.render_capability.path =
        stream.mode == WasapiOutputMode::Exclusive
        ? core::contract::CapabilityPath::DedicatedPath
        : core::contract::CapabilityPath::ExternalPath;
    capabilities_.render_capability.max_sample_rate_ceiling =
        stream.accepted_format.format.sample_rate;
    capabilities_.render_capability.max_channels_ceiling =
        stream.accepted_format.format.channel_count();
    capabilities_.buffer_capacity_frames = stream.buffer_frames;
    capabilities_.maximum_write_frames = stream.buffer_frames;
    capabilities_.feature_bits =
        core::render::render_feature_bit(core::render::RenderFeature::PartialWrite)
        | core::render::render_feature_bit(core::render::RenderFeature::DelayReport)
        | core::render::render_feature_bit(core::render::RenderFeature::Flush)
        | core::render::render_feature_bit(core::render::RenderFeature::Drain)
        | core::render::render_feature_bit(core::render::RenderFeature::Reset)
        | core::render::render_feature_bit(core::render::RenderFeature::ExclusiveMode);

    snapshot_.state = core::render::RenderLifecycleState::Open;
    snapshot_.accepted_format = stream.accepted_format;
    snapshot_.buffer_capacity_frames = stream.buffer_frames;
    snapshot_.buffered_frames = 0;
    snapshot_.total_accepted_frames = 0;
    snapshot_.generations = request.generations;
    snapshot_.generations.device = device_generation_;
    snapshot_.last_failure = core::render::RenderFailure::None;

    diagnostics_.mode = stream.mode;
    diagnostics_.mode_capabilities = WasapiRenderer::supported_modes();
    diagnostics_.event_driven = true;
    diagnostics_.reopen_required = false;
    diagnostics_.power_transition_count = power_observer_.transition_count();
    diagnostics_.default_device_period =
        reference_time_duration(default_device_period);
    diagnostics_.minimum_device_period =
        reference_time_duration(minimum_device_period);
    diagnostics_.buffer_duration = frame_duration(
        stream.buffer_frames,
        stream.accepted_format.format.sample_rate);
    diagnostics_.last_platform_code = 0;
    diagnostics_.device_generation = device_generation_;

    return core::render::RenderOpenResult::Accepted(
        stream.accepted_format,
        stream.buffer_frames,
        capabilities_);
}

} // namespace kivo::platform::windows::wasapi::detail
