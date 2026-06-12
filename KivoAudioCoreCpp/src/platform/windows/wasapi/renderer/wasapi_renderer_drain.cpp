#include "platform/windows/wasapi/renderer/wasapi_renderer_state.hpp"

#include <algorithm>
#include <limits>

namespace kivo::platform::windows::wasapi::detail {

namespace {

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

} // namespace

core::render::RenderControlResult WasapiRendererState::drain(
    const core::render::RenderDrainRequest& request) noexcept {
    using core::render::RenderControlResult;
    using core::render::RenderFailure;
    using core::render::RenderLifecycleState;

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

} // namespace kivo::platform::windows::wasapi::detail
