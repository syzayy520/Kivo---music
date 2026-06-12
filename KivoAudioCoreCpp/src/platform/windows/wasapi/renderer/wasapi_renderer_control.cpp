#include "platform/windows/wasapi/renderer/wasapi_renderer_state.hpp"

namespace kivo::platform::windows::wasapi::detail {

using core::render::RenderControlResult;
using core::render::RenderFailure;
using core::render::RenderLifecycleState;

RenderControlResult WasapiRendererState::start() noexcept {
    if (!on_control_thread()) {
        return wrong_thread_result();
    }
    if (detect_render_environment_change()) {
        return RenderControlResult::Failed(RenderFailure::DeviceLost);
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

RenderControlResult WasapiRendererState::flush(
    const core::render::RenderFlushRequest& request) noexcept {
    if (!on_control_thread()) {
        return wrong_thread_result();
    }
    if (detect_render_environment_change()) {
        return RenderControlResult::Failed(RenderFailure::DeviceLost);
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

RenderControlResult WasapiRendererState::reset(
    const core::render::RenderResetRequest& request) noexcept {
    if (!on_control_thread()) {
        return wrong_thread_result();
    }
    if (detect_render_environment_change()) {
        return RenderControlResult::Failed(RenderFailure::DeviceLost);
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

RenderControlResult WasapiRendererState::stop() noexcept {
    if (!on_control_thread()) {
        return wrong_thread_result();
    }
    if (detect_render_environment_change()) {
        return RenderControlResult::Failed(RenderFailure::DeviceLost);
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

RenderControlResult WasapiRendererState::close() noexcept {
    if (apartment_.is_initialized() && !on_control_thread()) {
        return wrong_thread_result();
    }
    close_stream();
    return RenderControlResult::Succeeded(RenderLifecycleState::Closed);
}

} // namespace kivo::platform::windows::wasapi::detail
