#include "kivo/platform/windows/wasapi/wasapi_renderer.hpp"

#include <Windows.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <objbase.h>
#include <wrl/client.h>

#include <algorithm>
#include <cstring>
#include <limits>
#include <new>

#include "com_apartment.hpp"
#include "wasapi_error.hpp"
#include "wasapi_format.hpp"

namespace kivo::platform::windows::wasapi {

namespace {

using Microsoft::WRL::ComPtr;
using core::contract::FrameCount;
using core::render::RenderControlResult;
using core::render::RenderFailure;
using core::render::RenderLifecycleState;
using core::render::RenderWriteResult;

constexpr REFERENCE_TIME kSharedBufferDuration = 1'000'000; // 100 ms

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

[[nodiscard]] FrameCount saturating_add(FrameCount left, FrameCount right) noexcept {
    const auto maximum = std::numeric_limits<FrameCount>::max();
    return right > maximum - left ? maximum : left + right;
}

[[nodiscard]] DWORD wait_milliseconds(std::chrono::milliseconds timeout) noexcept {
    if (timeout.count() <= 0) {
        return 0;
    }
    const auto maximum = static_cast<int64_t>(INFINITE - 1);
    return static_cast<DWORD>(std::min(timeout.count(), maximum));
}

[[nodiscard]] std::chrono::milliseconds ceil_milliseconds(
    std::chrono::nanoseconds timeout) noexcept {
    constexpr auto nanoseconds_per_millisecond =
        std::chrono::nanoseconds{std::chrono::milliseconds{1}}.count();
    const auto count = timeout.count();
    return std::chrono::milliseconds{
        count / nanoseconds_per_millisecond
            + (count % nanoseconds_per_millisecond == 0 ? 0 : 1)};
}

[[nodiscard]] uint64_t saturating_tick_deadline(DWORD timeout) noexcept {
    const auto now = GetTickCount64();
    const auto maximum = std::numeric_limits<uint64_t>::max();
    return timeout > maximum - now ? maximum : now + timeout;
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
    FrameCount frames,
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

class WasapiRenderer::Impl {
public:
    ~Impl() {
        close_stream();
    }

    [[nodiscard]] core::render::RendererIdentity identity() const noexcept {
        return capabilities_.identity;
    }

    [[nodiscard]] core::render::RendererCapabilitySnapshot capabilities() const noexcept {
        return capabilities_;
    }

    [[nodiscard]] core::render::RenderOpenResult open(
        const core::render::RenderOpenRequest& request) noexcept {
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
        const auto mapped = detail::map_wave_format(mix_format, accepted_format);
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

    [[nodiscard]] RenderControlResult start() noexcept {
        if (!on_control_thread()) {
            return wrong_thread_result();
        }
        if (snapshot_.state == RenderLifecycleState::Started) {
            return RenderControlResult::NoOp(snapshot_.state);
        }
        if (snapshot_.state != RenderLifecycleState::Open
            && snapshot_.state != RenderLifecycleState::Stopped) {
            return RenderControlResult::Rejected(RenderFailure::InvalidState, snapshot_.state);
        }

        const auto result = audio_client_->Start();
        if (FAILED(result)) {
            return fail_control(result);
        }
        has_submitted_since_start_ = false;
        snapshot_.state = RenderLifecycleState::Started;
        return RenderControlResult::Succeeded(snapshot_.state);
    }

    [[nodiscard]] RenderControlResult flush(
        const core::render::RenderFlushRequest& request) noexcept {
        if (!on_control_thread()) {
            return wrong_thread_result();
        }
        if (snapshot_.state == RenderLifecycleState::Closed
            || snapshot_.state == RenderLifecycleState::Failed) {
            return RenderControlResult::Rejected(RenderFailure::InvalidState, snapshot_.state);
        }

        const auto restart = snapshot_.state == RenderLifecycleState::Started
            || snapshot_.state == RenderLifecycleState::Draining;
        auto result = audio_client_->Stop();
        if (FAILED(result) && result != AUDCLNT_E_NOT_STOPPED) {
            return fail_control(result);
        }
        result = audio_client_->Reset();
        if (FAILED(result)) {
            return fail_control(result);
        }

        snapshot_.buffered_frames = 0;
        snapshot_.generations = request.generations;
        snapshot_.generations.device = device_generation_;
        snapshot_.last_failure = RenderFailure::None;
        snapshot_.state = RenderLifecycleState::Open;

        if (restart) {
            result = audio_client_->Start();
            if (FAILED(result)) {
                return fail_control(result);
            }
            has_submitted_since_start_ = false;
            snapshot_.state = RenderLifecycleState::Started;
        }
        return RenderControlResult::Succeeded(snapshot_.state);
    }

    [[nodiscard]] RenderControlResult drain(
        const core::render::RenderDrainRequest& request) noexcept {
        if (!on_control_thread()) {
            return wrong_thread_result();
        }
        if (!request.is_valid()) {
            return RenderControlResult::Rejected(RenderFailure::Timeout, snapshot_.state);
        }
        if (snapshot_.state != RenderLifecycleState::Started
            && snapshot_.state != RenderLifecycleState::Draining) {
            return RenderControlResult::Rejected(RenderFailure::InvalidState, snapshot_.state);
        }

        snapshot_.state = RenderLifecycleState::Draining;
        const auto timeout_ms = ceil_milliseconds(request.timeout);
        const auto deadline = saturating_tick_deadline(wait_milliseconds(timeout_ms));

        while (true) {
            UINT32 padding = 0;
            const auto padding_result = audio_client_->GetCurrentPadding(&padding);
            if (FAILED(padding_result)) {
                return fail_control(padding_result);
            }
            snapshot_.buffered_frames = padding;
            if (padding == 0) {
                const auto stop_result = audio_client_->Stop();
                if (FAILED(stop_result) && stop_result != AUDCLNT_E_NOT_STOPPED) {
                    return fail_control(stop_result);
                }
                snapshot_.state = RenderLifecycleState::Stopped;
                snapshot_.last_failure = RenderFailure::None;
                return RenderControlResult::Succeeded(snapshot_.state);
            }

            const auto now = GetTickCount64();
            if (now >= deadline) {
                snapshot_.last_failure = RenderFailure::Timeout;
                return RenderControlResult::TimedOut(snapshot_.state);
            }
            const auto remaining = static_cast<DWORD>(
                std::min<uint64_t>(deadline - now, INFINITE - 1));
            const auto wait_result = WaitForSingleObject(event_handle_, remaining);
            if (wait_result == WAIT_TIMEOUT) {
                snapshot_.last_failure = RenderFailure::Timeout;
                return RenderControlResult::TimedOut(snapshot_.state);
            }
            if (wait_result != WAIT_OBJECT_0) {
                return fail_control(HRESULT_FROM_WIN32(GetLastError()));
            }
        }
    }

    [[nodiscard]] RenderControlResult reset(
        const core::render::RenderResetRequest& request) noexcept {
        if (!on_control_thread()) {
            return wrong_thread_result();
        }
        if (snapshot_.state == RenderLifecycleState::Closed) {
            return RenderControlResult::Rejected(RenderFailure::InvalidState, snapshot_.state);
        }

        auto result = audio_client_->Stop();
        if (FAILED(result) && result != AUDCLNT_E_NOT_STOPPED) {
            return fail_control(result);
        }
        result = audio_client_->Reset();
        if (FAILED(result)) {
            return fail_control(result);
        }

        snapshot_.state = RenderLifecycleState::Open;
        snapshot_.buffered_frames = 0;
        has_submitted_since_start_ = false;
        snapshot_.generations = request.generations;
        snapshot_.generations.device = device_generation_;
        snapshot_.last_failure = RenderFailure::None;
        return RenderControlResult::Succeeded(snapshot_.state);
    }

    [[nodiscard]] RenderControlResult stop() noexcept {
        if (!on_control_thread()) {
            return wrong_thread_result();
        }
        if (snapshot_.state == RenderLifecycleState::Stopped) {
            return RenderControlResult::NoOp(snapshot_.state);
        }
        if (snapshot_.state == RenderLifecycleState::Closed
            || snapshot_.state == RenderLifecycleState::Failed) {
            return RenderControlResult::Rejected(RenderFailure::InvalidState, snapshot_.state);
        }

        const auto result = audio_client_->Stop();
        if (FAILED(result) && result != AUDCLNT_E_NOT_STOPPED) {
            return fail_control(result);
        }
        snapshot_.state = RenderLifecycleState::Stopped;
        return RenderControlResult::Succeeded(snapshot_.state);
    }

    [[nodiscard]] RenderControlResult close() noexcept {
        if (apartment_.is_initialized() && !on_control_thread()) {
            return wrong_thread_result();
        }
        close_stream();
        return RenderControlResult::Succeeded(RenderLifecycleState::Closed);
    }

    [[nodiscard]] core::render::RenderSnapshot snapshot() const noexcept {
        auto current = snapshot_;
        if (audio_client_ != nullptr
            && current.state != RenderLifecycleState::Closed
            && current.state != RenderLifecycleState::Failed) {
            UINT32 padding = 0;
            if (SUCCEEDED(audio_client_->GetCurrentPadding(&padding))) {
                current.buffered_frames = padding;
            }
        }
        return current;
    }

    [[nodiscard]] RenderWriteResult write(
        const core::render::AudioBlockView& block) noexcept {
        if (snapshot_.state != RenderLifecycleState::Started) {
            return RenderWriteResult::Rejected(block.frame_count, RenderFailure::InvalidState);
        }
        if (!block.is_valid()) {
            return RenderWriteResult::Rejected(block.frame_count, RenderFailure::InvalidBuffer);
        }
        if (block.format != snapshot_.accepted_format) {
            return RenderWriteResult::Rejected(block.frame_count, RenderFailure::UnsupportedFormat);
        }

        const auto generation_failure = stale_generation(block.generations);
        if (generation_failure != RenderFailure::None) {
            return RenderWriteResult::Rejected(block.frame_count, generation_failure);
        }

        UINT32 padding = 0;
        auto result = audio_client_->GetCurrentPadding(&padding);
        if (FAILED(result)) {
            return fail_write(result, block.frame_count);
        }
        if (padding > snapshot_.buffer_capacity_frames) {
            return fail_write(AUDCLNT_E_BUFFER_SIZE_ERROR, block.frame_count);
        }
        if (padding == 0 && has_submitted_since_start_) {
            ++diagnostics_.underrun_count;
            snapshot_.last_failure = RenderFailure::Underrun;
        }

        const auto available = snapshot_.buffer_capacity_frames - padding;
        const auto accepted = std::min<FrameCount>(block.frame_count, available);
        if (accepted == 0) {
            return RenderWriteResult::Delayed(
                block.frame_count,
                std::chrono::milliseconds{1});
        }

        BYTE* destination = nullptr;
        result = render_client_->GetBuffer(static_cast<UINT32>(accepted), &destination);
        if (FAILED(result)) {
            return fail_write(result, block.frame_count);
        }

        const auto bytes_per_frame = block.format.format.bytes_per_frame();
        const auto byte_count = static_cast<size_t>(accepted) * bytes_per_frame;
        std::memcpy(destination, block.bytes.data(), byte_count);
        result = render_client_->ReleaseBuffer(static_cast<UINT32>(accepted), 0);
        if (FAILED(result)) {
            return fail_write(result, block.frame_count);
        }

        snapshot_.buffered_frames = padding + accepted;
        snapshot_.total_accepted_frames =
            saturating_add(snapshot_.total_accepted_frames, accepted);
        has_submitted_since_start_ = true;
        return accepted == block.frame_count
            ? RenderWriteResult::Full(block.frame_count)
            : RenderWriteResult::Partial(block.frame_count, accepted);
    }

    [[nodiscard]] WasapiWaitResult wait_until_writable(
        std::chrono::milliseconds timeout) noexcept {
        if (event_handle_ == nullptr || snapshot_.state == RenderLifecycleState::Closed) {
            return WasapiWaitResult::Closed;
        }
        if (snapshot_.state == RenderLifecycleState::Failed) {
            return snapshot_.last_failure == RenderFailure::DeviceLost
                ? WasapiWaitResult::DeviceLost
                : WasapiWaitResult::Failed;
        }

        const auto result = WaitForSingleObject(event_handle_, wait_milliseconds(timeout));
        if (result == WAIT_OBJECT_0) {
            return WasapiWaitResult::Ready;
        }
        if (result == WAIT_TIMEOUT) {
            return WasapiWaitResult::Timeout;
        }
        diagnostics_.last_platform_code = GetLastError();
        return WasapiWaitResult::Failed;
    }

    [[nodiscard]] WasapiDiagnostics diagnostics() const noexcept {
        return diagnostics_;
    }

private:
    [[nodiscard]] bool on_control_thread() const noexcept {
        return control_thread_id_ != 0 && control_thread_id_ == GetCurrentThreadId();
    }

    [[nodiscard]] RenderControlResult wrong_thread_result() noexcept {
        diagnostics_.last_platform_code = RPC_E_WRONG_THREAD;
        return RenderControlResult::Rejected(RenderFailure::InvalidState, snapshot_.state);
    }

    [[nodiscard]] RenderFailure stale_generation(
        const core::render::RenderGenerationSet& generations) const noexcept {
        if (generations.stream != snapshot_.generations.stream) {
            return RenderFailure::StaleStreamGeneration;
        }
        if (generations.seek != snapshot_.generations.seek) {
            return RenderFailure::StaleSeekGeneration;
        }
        if (generations.flush != snapshot_.generations.flush) {
            return RenderFailure::StaleFlushGeneration;
        }
        if (generations.device != snapshot_.generations.device) {
            return RenderFailure::StaleDeviceGeneration;
        }
        return RenderFailure::None;
    }

    [[nodiscard]] core::render::RenderOpenResult fail_open(HRESULT result) noexcept {
        const auto failure = detail::map_wasapi_error(result);
        diagnostics_.last_platform_code = result;
        if (detail::is_device_invalidation(result)) {
            ++diagnostics_.device_invalidation_count;
            diagnostics_.reopen_required = true;
        }
        close_stream();
        return core::render::RenderOpenResult::Failed(failure);
    }

    [[nodiscard]] RenderControlResult fail_control(HRESULT result) noexcept {
        const auto failure = detail::map_wasapi_error(result);
        diagnostics_.last_platform_code = result;
        snapshot_.last_failure = failure;
        if (detail::is_device_invalidation(result)) {
            ++diagnostics_.device_invalidation_count;
            diagnostics_.reopen_required = true;
            device_generation_.id =
                core::contract::GenerationId::next(device_generation_.id);
            diagnostics_.device_generation = device_generation_;
        }
        snapshot_.state = RenderLifecycleState::Failed;
        return RenderControlResult::Failed(failure);
    }

    [[nodiscard]] RenderWriteResult fail_write(
        HRESULT result,
        FrameCount submitted_frames) noexcept {
        const auto failure = detail::map_wasapi_error(result);
        diagnostics_.last_platform_code = result;
        snapshot_.last_failure = failure;
        if (detail::is_device_invalidation(result)) {
            ++diagnostics_.device_invalidation_count;
            diagnostics_.reopen_required = true;
            device_generation_.id =
                core::contract::GenerationId::next(device_generation_.id);
            diagnostics_.device_generation = device_generation_;
        }
        snapshot_.state = RenderLifecycleState::Failed;
        return RenderWriteResult::Failed(submitted_frames, failure);
    }

    void close_stream() noexcept {
        if (audio_client_ != nullptr) {
            static_cast<void>(audio_client_->Stop());
        }
        render_client_.Reset();
        audio_client_.Reset();
        device_.Reset();
        enumerator_.Reset();
        if (event_handle_ != nullptr) {
            CloseHandle(event_handle_);
            event_handle_ = nullptr;
        }
        apartment_.uninitialize();
        control_thread_id_ = 0;
        has_submitted_since_start_ = false;
        snapshot_ = {};
        diagnostics_.mode = WasapiOutputMode::Closed;
        diagnostics_.mode_capabilities = WasapiRenderer::supported_modes();
        diagnostics_.event_driven = false;
        diagnostics_.device_generation = device_generation_;
    }

    detail::ComApartment apartment_;
    ComPtr<IMMDeviceEnumerator> enumerator_;
    ComPtr<IMMDevice> device_;
    ComPtr<IAudioClient> audio_client_;
    ComPtr<IAudioRenderClient> render_client_;
    HANDLE event_handle_{nullptr};
    DWORD control_thread_id_{0};
    core::contract::DeviceGeneration device_generation_{};
    bool opened_before_{false};
    bool has_submitted_since_start_{false};
    core::render::RendererCapabilitySnapshot capabilities_{};
    core::render::RenderSnapshot snapshot_{};
    WasapiDiagnostics diagnostics_{};
};

WasapiRenderer::WasapiRenderer()
    : impl_(std::make_unique<Impl>()) {}

WasapiRenderer::~WasapiRenderer() = default;

core::render::RendererIdentity WasapiRenderer::identity() const noexcept {
    return impl_ ? impl_->identity() : core::render::RendererIdentity{};
}

core::render::RendererCapabilitySnapshot WasapiRenderer::capabilities() const noexcept {
    return impl_ ? impl_->capabilities() : core::render::RendererCapabilitySnapshot{};
}

core::render::RenderOpenResult WasapiRenderer::open(
    const core::render::RenderOpenRequest& request) noexcept {
    return impl_
        ? impl_->open(request)
        : core::render::RenderOpenResult::Failed(RenderFailure::BoundaryFailure);
}

RenderControlResult WasapiRenderer::start() noexcept {
    return impl_
        ? impl_->start()
        : RenderControlResult::Failed(RenderFailure::BoundaryFailure);
}

RenderControlResult WasapiRenderer::flush(
    const core::render::RenderFlushRequest& request) noexcept {
    return impl_
        ? impl_->flush(request)
        : RenderControlResult::Failed(RenderFailure::BoundaryFailure);
}

RenderControlResult WasapiRenderer::drain(
    const core::render::RenderDrainRequest& request) noexcept {
    return impl_
        ? impl_->drain(request)
        : RenderControlResult::Failed(RenderFailure::BoundaryFailure);
}

RenderControlResult WasapiRenderer::reset(
    const core::render::RenderResetRequest& request) noexcept {
    return impl_
        ? impl_->reset(request)
        : RenderControlResult::Failed(RenderFailure::BoundaryFailure);
}

RenderControlResult WasapiRenderer::stop() noexcept {
    return impl_
        ? impl_->stop()
        : RenderControlResult::Failed(RenderFailure::BoundaryFailure);
}

RenderControlResult WasapiRenderer::close() noexcept {
    return impl_
        ? impl_->close()
        : RenderControlResult::Succeeded(RenderLifecycleState::Closed);
}

core::render::RenderSnapshot WasapiRenderer::snapshot() const noexcept {
    return impl_ ? impl_->snapshot() : core::render::RenderSnapshot{};
}

RenderWriteResult WasapiRenderer::write(
    const core::render::AudioBlockView& block) noexcept {
    return impl_
        ? impl_->write(block)
        : RenderWriteResult::Failed(block.frame_count, RenderFailure::BoundaryFailure);
}

WasapiWaitResult WasapiRenderer::wait_until_writable(
    std::chrono::milliseconds timeout) noexcept {
    return impl_ ? impl_->wait_until_writable(timeout) : WasapiWaitResult::Closed;
}

WasapiDiagnostics WasapiRenderer::diagnostics() const noexcept {
    return impl_ ? impl_->diagnostics() : WasapiDiagnostics{};
}

} // namespace kivo::platform::windows::wasapi
