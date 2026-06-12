#include "platform/windows/wasapi/renderer/wasapi_renderer_state.hpp"

#include <objbase.h>

#include <algorithm>
#include <limits>
#include <new>

#include "platform/windows/wasapi/format/wasapi_format.hpp"

namespace kivo::platform::windows::wasapi::detail {

namespace {

constexpr REFERENCE_TIME kSharedBufferDuration = 1'000'000;

[[nodiscard]] uint64_t stable_endpoint_hash(const wchar_t* value) noexcept {
    constexpr uint64_t offset_basis = 14695981039346656037ull;
    constexpr uint64_t prime = 1099511628211ull;
    uint64_t hash = offset_basis;
    if (value != nullptr) {
        while (*value != L'\0') {
            hash ^= static_cast<uint16_t>(*value);
            hash *= prime;
            ++value;
        }
    }
    return hash == 0 ? 1 : hash;
}

[[nodiscard]] std::chrono::nanoseconds reference_time_duration(
    REFERENCE_TIME value) noexcept {
    if (value <= 0) {
        return {};
    }
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
    if (sample_rate == 0) {
        return {};
    }
    constexpr uint64_t nanoseconds_per_second = 1'000'000'000ull;
    const auto numerator = static_cast<uint64_t>(frames) * nanoseconds_per_second;
    const auto count = numerator / sample_rate
        + (numerator % sample_rate == 0 ? 0 : 1);
    return std::chrono::nanoseconds{static_cast<int64_t>(count)};
}

} // namespace

core::render::RenderOpenResult WasapiRendererState::open(
    const core::render::RenderOpenRequest& request) noexcept {
    using core::render::RenderFailure;
    using core::render::RenderLifecycleState;

    if (snapshot_.state != RenderLifecycleState::Closed) {
        return core::render::RenderOpenResult::Rejected(RenderFailure::InvalidState);
    }
    if (!request.is_valid()) {
        return core::render::RenderOpenResult::Rejected(RenderFailure::InvalidFormat);
    }

    auto result = apartment_.initialize();
    if (FAILED(result)) {
        return fail_open(result);
    }

    result = CoCreateInstance(
        __uuidof(MMDeviceEnumerator),
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&enumerator_));
    if (FAILED(result)) {
        return fail_open(result);
    }

    result = enumerator_->GetDefaultAudioEndpoint(eRender, eConsole, &device_);
    if (FAILED(result)) {
        return fail_open(result);
    }

    LPWSTR endpoint_id = nullptr;
    result = device_->GetId(&endpoint_id);
    if (FAILED(result)) {
        return fail_open(result);
    }
    const auto endpoint_identity = stable_endpoint_hash(endpoint_id);
    endpoint_observer_.Attach(
        new (std::nothrow) DefaultRenderEndpointObserver(endpoint_id));
    if (endpoint_observer_ == nullptr || !endpoint_observer_->is_valid()) {
        CoTaskMemFree(endpoint_id);
        return fail_open(E_OUTOFMEMORY);
    }
    result = enumerator_->RegisterEndpointNotificationCallback(
        endpoint_observer_.Get());
    if (FAILED(result)) {
        CoTaskMemFree(endpoint_id);
        return fail_open(result);
    }
    endpoint_observer_registered_ = true;
    CoTaskMemFree(endpoint_id);

    result = device_->Activate(
        __uuidof(IAudioClient),
        CLSCTX_INPROC_SERVER,
        nullptr,
        reinterpret_cast<void**>(audio_client_.GetAddressOf()));
    if (FAILED(result)) {
        return fail_open(result);
    }

    WAVEFORMATEX* mix_format = nullptr;
    result = audio_client_->GetMixFormat(&mix_format);
    if (FAILED(result)) {
        return fail_open(result);
    }

    REFERENCE_TIME default_device_period = 0;
    REFERENCE_TIME minimum_device_period = 0;
    result = audio_client_->GetDevicePeriod(
        &default_device_period,
        &minimum_device_period);
    if (FAILED(result)
        || default_device_period <= 0
        || minimum_device_period <= 0) {
        CoTaskMemFree(mix_format);
        return fail_open(FAILED(result) ? result : E_UNEXPECTED);
    }

    core::contract::RenderFormat accepted_format{};
    const auto mapped = map_wave_format(mix_format, accepted_format);
    if (!mapped) {
        CoTaskMemFree(mix_format);
        return fail_open(AUDCLNT_E_UNSUPPORTED_FORMAT);
    }
    if (request.format_policy == core::render::FormatAcceptancePolicy::Exact
        && request.requested_format != accepted_format) {
        CoTaskMemFree(mix_format);
        close_stream();
        return core::render::RenderOpenResult::Rejected(RenderFailure::UnsupportedFormat);
    }

    event_handle_ = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (event_handle_ == nullptr) {
        CoTaskMemFree(mix_format);
        return fail_open(HRESULT_FROM_WIN32(GetLastError()));
    }

    result = audio_client_->Initialize(
        AUDCLNT_SHAREMODE_SHARED,
        AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST,
        kSharedBufferDuration,
        0,
        mix_format,
        nullptr);
    CoTaskMemFree(mix_format);
    if (FAILED(result)) {
        return fail_open(result);
    }

    result = audio_client_->SetEventHandle(event_handle_);
    if (FAILED(result)) {
        return fail_open(result);
    }

    UINT32 buffer_frames = 0;
    result = audio_client_->GetBufferSize(&buffer_frames);
    if (FAILED(result) || buffer_frames == 0) {
        return fail_open(FAILED(result) ? result : E_FAIL);
    }

    result = audio_client_->GetService(
        __uuidof(IAudioRenderClient),
        reinterpret_cast<void**>(render_client_.GetAddressOf()));
    if (FAILED(result)) {
        return fail_open(result);
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
        core::contract::CapabilityPath::ExternalPath;
    capabilities_.render_capability.max_sample_rate_ceiling =
        accepted_format.format.sample_rate;
    capabilities_.render_capability.max_channels_ceiling =
        accepted_format.format.channel_count();
    capabilities_.buffer_capacity_frames = buffer_frames;
    capabilities_.maximum_write_frames = buffer_frames;
    capabilities_.feature_bits =
        core::render::render_feature_bit(core::render::RenderFeature::PartialWrite)
        | core::render::render_feature_bit(core::render::RenderFeature::DelayReport)
        | core::render::render_feature_bit(core::render::RenderFeature::Flush)
        | core::render::render_feature_bit(core::render::RenderFeature::Drain)
        | core::render::render_feature_bit(core::render::RenderFeature::Reset);

    snapshot_.state = RenderLifecycleState::Open;
    snapshot_.accepted_format = accepted_format;
    snapshot_.buffer_capacity_frames = buffer_frames;
    snapshot_.buffered_frames = 0;
    snapshot_.total_accepted_frames = 0;
    snapshot_.generations = request.generations;
    snapshot_.generations.device = device_generation_;
    snapshot_.last_failure = RenderFailure::None;

    diagnostics_.mode = WasapiOutputMode::Shared;
    diagnostics_.mode_capabilities = WasapiRenderer::supported_modes();
    diagnostics_.event_driven = true;
    diagnostics_.reopen_required = false;
    diagnostics_.default_device_period =
        reference_time_duration(default_device_period);
    diagnostics_.minimum_device_period =
        reference_time_duration(minimum_device_period);
    diagnostics_.buffer_duration =
        frame_duration(buffer_frames, accepted_format.format.sample_rate);
    diagnostics_.last_platform_code = 0;
    diagnostics_.device_generation = device_generation_;

    return core::render::RenderOpenResult::Accepted(
        accepted_format,
        buffer_frames,
        capabilities_);
}

} // namespace kivo::platform::windows::wasapi::detail
