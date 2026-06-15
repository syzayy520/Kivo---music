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
    return publication_.read();
}

WasapiWaitResult WasapiRendererState::wait_until_writable(
    std::chrono::milliseconds timeout) noexcept {
    const auto current = publication_.read();
    if (event_handle_ == nullptr
        || current.state == core::render::RenderLifecycleState::Closed) {
        return WasapiWaitResult::Closed;
    }
    if (current.state == core::render::RenderLifecycleState::Failed) {
        return current.last_failure == core::render::RenderFailure::DeviceLost
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
    return WasapiWaitResult::Failed;
}

bool WasapiRendererState::is_started_for_worker() const noexcept {
    return publication_.read().state
        == core::render::RenderLifecycleState::Started;
}

WasapiDiagnostics WasapiRendererState::diagnostics() const noexcept {
    return diagnostics_;
}

} // namespace kivo::platform::windows::wasapi::detail
