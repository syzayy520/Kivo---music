#include "platform/windows/wasapi/worker/wasapi_render_worker_runtime.hpp"

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

WasapiRenderWorkerRuntime::WasapiRenderWorkerRuntime(
    WasapiRenderer& renderer,
    core::render::SpscAudioBlockQueue& queue) noexcept
    : renderer_(renderer), queue_(queue) {}

WasapiRenderWorkerRuntime::~WasapiRenderWorkerRuntime() {
    request_stop();
    join_forever();
}

bool WasapiRenderWorkerRuntime::start() noexcept {
    if (thread_handle_ != nullptr
        || renderer_.snapshot().state != core::render::RenderLifecycleState::Started) {
        return false;
    }

    stop_requested_.store(false, std::memory_order_release);
    state_.store(WasapiRenderWorkerState::Running, std::memory_order_release);
    last_failure_.store(core::render::RenderFailure::None, std::memory_order_relaxed);
    last_wait_result_.store(WasapiWaitResult::Closed, std::memory_order_relaxed);
    mmcss_registered_.store(false, std::memory_order_relaxed);
    mmcss_high_priority_.store(false, std::memory_order_relaxed);
    accepted_frames_.store(0, std::memory_order_relaxed);
    discarded_stale_blocks_.store(0, std::memory_order_relaxed);
    wait_timeout_count_.store(0, std::memory_order_relaxed);
    last_platform_code_.store(0, std::memory_order_relaxed);

    thread_handle_ = CreateThread(
        nullptr,
        0,
        &WasapiRenderWorkerRuntime::thread_entry,
        this,
        0,
        nullptr);
    if (thread_handle_ == nullptr) {
        last_platform_code_.store(GetLastError(), std::memory_order_relaxed);
        last_failure_.store(
            core::render::RenderFailure::BoundaryFailure,
            std::memory_order_relaxed);
        state_.store(WasapiRenderWorkerState::Failed, std::memory_order_release);
        return false;
    }
    return true;
}

void WasapiRenderWorkerRuntime::request_stop() noexcept {
    stop_requested_.store(true, std::memory_order_release);
    auto expected = WasapiRenderWorkerState::Running;
    static_cast<void>(state_.compare_exchange_strong(
        expected,
        WasapiRenderWorkerState::Stopping,
        std::memory_order_acq_rel,
        std::memory_order_acquire));
}

bool WasapiRenderWorkerRuntime::join(std::chrono::milliseconds timeout) noexcept {
    if (thread_handle_ == nullptr) {
        return true;
    }
    const auto wait_result =
        WaitForSingleObject(thread_handle_, wait_milliseconds(timeout));
    if (wait_result != WAIT_OBJECT_0) {
        if (wait_result != WAIT_TIMEOUT) {
            last_platform_code_.store(GetLastError(), std::memory_order_relaxed);
        }
        return false;
    }
    CloseHandle(thread_handle_);
    thread_handle_ = nullptr;
    return true;
}

WasapiRenderWorkerSnapshot WasapiRenderWorkerRuntime::snapshot() const noexcept {
    return {
        state_.load(std::memory_order_acquire),
        last_failure_.load(std::memory_order_relaxed),
        last_wait_result_.load(std::memory_order_relaxed),
        mmcss_registered_.load(std::memory_order_relaxed),
        mmcss_high_priority_.load(std::memory_order_relaxed),
        accepted_frames_.load(std::memory_order_relaxed),
        discarded_stale_blocks_.load(std::memory_order_relaxed),
        wait_timeout_count_.load(std::memory_order_relaxed),
        last_platform_code_.load(std::memory_order_relaxed)
    };
}

void WasapiRenderWorkerRuntime::join_forever() noexcept {
    if (thread_handle_ == nullptr) {
        return;
    }
    static_cast<void>(WaitForSingleObject(thread_handle_, INFINITE));
    CloseHandle(thread_handle_);
    thread_handle_ = nullptr;
}

} // namespace kivo::platform::windows::wasapi::detail
