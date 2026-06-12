#include "platform/windows/wasapi/renderer/wasapi_renderer_state.hpp"

#include <algorithm>

namespace kivo::platform::windows::wasapi::detail {

namespace {

[[nodiscard]] DWORD wait_milliseconds(std::chrono::milliseconds timeout) noexcept {
    if (timeout.count() <= 0) {
        return 0;
    }
    const auto maximum = static_cast<int64_t>(INFINITE - 1);
    return static_cast<DWORD>(std::min(timeout.count(), maximum));
}

} // namespace

core::render::RendererIdentity WasapiRendererState::identity() const noexcept {
    return capabilities_.identity;
}

core::render::RendererCapabilitySnapshot
WasapiRendererState::capabilities() const noexcept {
    return capabilities_;
}

core::render::RenderSnapshot WasapiRendererState::snapshot() const noexcept {
    auto current = snapshot_;
    if (audio_client_ != nullptr
        && current.state != core::render::RenderLifecycleState::Closed
        && current.state != core::render::RenderLifecycleState::Failed) {
        UINT32 padding = 0;
        if (SUCCEEDED(audio_client_->GetCurrentPadding(&padding))) {
            current.buffered_frames = padding;
        }
    }
    return current;
}

WasapiWaitResult WasapiRendererState::wait_until_writable(
    std::chrono::milliseconds timeout) noexcept {
    if (event_handle_ == nullptr
        || snapshot_.state == core::render::RenderLifecycleState::Closed) {
        return WasapiWaitResult::Closed;
    }
    if (snapshot_.state == core::render::RenderLifecycleState::Failed) {
        return snapshot_.last_failure == core::render::RenderFailure::DeviceLost
            ? WasapiWaitResult::DeviceLost
            : WasapiWaitResult::Failed;
    }

    const auto result =
        WaitForSingleObject(event_handle_, wait_milliseconds(timeout));
    if (result == WAIT_OBJECT_0) {
        return WasapiWaitResult::Ready;
    }
    if (result == WAIT_TIMEOUT) {
        return WasapiWaitResult::Timeout;
    }
    diagnostics_.last_platform_code = GetLastError();
    return WasapiWaitResult::Failed;
}

WasapiDiagnostics WasapiRendererState::diagnostics() const noexcept {
    return diagnostics_;
}

} // namespace kivo::platform::windows::wasapi::detail
