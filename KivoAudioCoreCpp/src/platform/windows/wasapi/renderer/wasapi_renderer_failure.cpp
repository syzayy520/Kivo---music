#include "platform/windows/wasapi/renderer/wasapi_renderer_state.hpp"

#include "platform/windows/wasapi/error/wasapi_error.hpp"

namespace kivo::platform::windows::wasapi::detail {

WasapiRendererState::~WasapiRendererState() {
    close_stream();
}

bool WasapiRendererState::on_control_thread() const noexcept {
    const auto control_thread_id =
        control_thread_id_.load(std::memory_order_relaxed);
    return control_thread_id != 0 && control_thread_id == GetCurrentThreadId();
}

core::render::RenderControlResult
WasapiRendererState::wrong_thread_result() noexcept {
    return core::render::RenderControlResult::Rejected(
        core::render::RenderFailure::InvalidState,
        publication_.read().state);
}

core::render::RenderFailure WasapiRendererState::stale_generation(
    const core::render::RenderGenerationSet& generations,
    const core::render::RenderGenerationSet& current) const noexcept {
    using core::render::RenderFailure;
    if (generations.stream != current.stream) {
        return RenderFailure::StaleStreamGeneration;
    }
    if (generations.seek != current.seek) {
        return RenderFailure::StaleSeekGeneration;
    }
    if (generations.flush != current.flush) {
        return RenderFailure::StaleFlushGeneration;
    }
    if (generations.device != current.device) {
        return RenderFailure::StaleDeviceGeneration;
    }
    return RenderFailure::None;
}

core::render::RenderOpenResult WasapiRendererState::fail_open(
    HRESULT result) noexcept {
    const auto failure = map_wasapi_error(result);
    diagnostics_.last_platform_code = result;
    if (failure == core::render::RenderFailure::Overrun) {
        ++diagnostics_.overrun_count;
    }
    if (is_device_invalidation(result)) {
        ++diagnostics_.device_invalidation_count;
        diagnostics_.reopen_required = true;
    }
    close_stream();
    return core::render::RenderOpenResult::Failed(failure);
}

core::render::RenderControlResult WasapiRendererState::fail_control(
    HRESULT result) noexcept {
    const auto failure = map_wasapi_error(result);
    diagnostics_.last_platform_code = result;
    snapshot_.last_failure = failure;
    if (failure == core::render::RenderFailure::Overrun) {
        ++diagnostics_.overrun_count;
    }
    if (is_device_invalidation(result)) {
        ++diagnostics_.device_invalidation_count;
        diagnostics_.reopen_required = true;
        device_generation_.id =
            core::contract::GenerationId::next(device_generation_.id);
        diagnostics_.device_generation = device_generation_;
    }
    snapshot_.state = core::render::RenderLifecycleState::Failed;
    publish_snapshot();
    return core::render::RenderControlResult::Failed(failure);
}

core::render::RenderWriteResult WasapiRendererState::fail_write(
    HRESULT result,
    core::contract::FrameCount submitted_frames) noexcept {
    const auto failure = map_wasapi_error(result);
    if (on_control_thread()) {
        diagnostics_.last_platform_code = result;
        snapshot_.last_failure = failure;
        if (failure == core::render::RenderFailure::Overrun) {
            ++diagnostics_.overrun_count;
        }
        if (is_device_invalidation(result)) {
            ++diagnostics_.device_invalidation_count;
            diagnostics_.reopen_required = true;
            device_generation_.id =
                core::contract::GenerationId::next(device_generation_.id);
            diagnostics_.device_generation = device_generation_;
        }
        snapshot_.state = core::render::RenderLifecycleState::Failed;
        publish_snapshot();
    } else {
        publication_.publish_failed_state(failure);
    }
    return core::render::RenderWriteResult::Failed(submitted_frames, failure);
}

void WasapiRendererState::sync_published_snapshot() noexcept {
    publication_.sync_into(snapshot_);
}

void WasapiRendererState::publish_snapshot() noexcept {
    publication_.publish(snapshot_);
}

void WasapiRendererState::close_stream() noexcept {
    power_observer_.unregister_callback();
    if (audio_client_ != nullptr) {
        static_cast<void>(audio_client_->Stop());
    }
    if (endpoint_observer_registered_ && enumerator_ != nullptr) {
        static_cast<void>(enumerator_->UnregisterEndpointNotificationCallback(
            endpoint_observer_.Get()));
    }
    endpoint_observer_registered_ = false;
    endpoint_observer_.Reset();
    render_client_.Reset();
    audio_client_.Reset();
    device_.Reset();
    enumerator_.Reset();
    if (event_handle_ != nullptr) {
        CloseHandle(event_handle_);
        event_handle_ = nullptr;
    }
    apartment_.uninitialize();
    control_thread_id_.store(0, std::memory_order_relaxed);
    has_submitted_since_start_.store(false, std::memory_order_relaxed);
    snapshot_ = {};
    publish_snapshot();
    diagnostics_.mode = WasapiOutputMode::Closed;
    diagnostics_.mode_capabilities = WasapiRenderer::supported_modes();
    diagnostics_.event_driven = false;
    diagnostics_.device_generation = device_generation_;
}

} // namespace kivo::platform::windows::wasapi::detail
