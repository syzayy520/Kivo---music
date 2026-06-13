#include "platform/windows/wasapi/worker/wasapi_render_worker_runtime.hpp"

#include <avrt.h>

#include <limits>

#include "kivo/core/render/pump/render_pump.hpp"

namespace kivo::platform::windows::wasapi::detail {

namespace {

template <typename Value>
void saturating_increment(std::atomic<Value>& value, Value amount = 1) noexcept {
    auto current = value.load(std::memory_order_relaxed);
    while (true) {
        const auto maximum = std::numeric_limits<Value>::max();
        const auto next = amount > maximum - current ? maximum : current + amount;
        if (value.compare_exchange_weak(
                current,
                next,
                std::memory_order_relaxed,
                std::memory_order_relaxed)) {
            return;
        }
    }
}

} // namespace

DWORD WINAPI WasapiRenderWorkerRuntime::thread_entry(void* context) noexcept {
    static_cast<WasapiRenderWorkerRuntime*>(context)->run();
    return 0;
}

void WasapiRenderWorkerRuntime::run() noexcept {
    DWORD task_index = 0;
    const auto mmcss_handle =
        AvSetMmThreadCharacteristicsW(L"Pro Audio", &task_index);
    if (mmcss_handle != nullptr) {
        mmcss_registered_.store(true, std::memory_order_relaxed);
        const auto high_priority =
            AvSetMmThreadPriority(mmcss_handle, AVRT_PRIORITY_HIGH) != FALSE;
        mmcss_high_priority_.store(high_priority, std::memory_order_relaxed);
        if (!high_priority) {
            last_platform_code_.store(GetLastError(), std::memory_order_relaxed);
        }
    } else {
        last_platform_code_.store(GetLastError(), std::memory_order_relaxed);
    }

    core::render::RenderPump pump(queue_, renderer_);
    while (!stop_requested_.load(std::memory_order_acquire)) {
        const auto result = pump.step();
        switch (result.disposition) {
        case core::render::RenderPumpDisposition::Progress:
            saturating_increment(accepted_frames_, result.accepted_frames);
            continue;

        case core::render::RenderPumpDisposition::EndOfStream:
            saturating_increment(accepted_frames_, result.accepted_frames);
            state_.store(
                WasapiRenderWorkerState::EndOfStream,
                std::memory_order_release);
            revert_mmcss(mmcss_handle);
            return;

        case core::render::RenderPumpDisposition::DiscardedStale:
            saturating_increment(discarded_stale_blocks_);
            continue;

        case core::render::RenderPumpDisposition::QueueDrained:
            state_.store(
                WasapiRenderWorkerState::QueueDrained,
                std::memory_order_release);
            revert_mmcss(mmcss_handle);
            return;

        case core::render::RenderPumpDisposition::Idle:
        case core::render::RenderPumpDisposition::Delayed:
            if (!wait_for_device()) {
                revert_mmcss(mmcss_handle);
                return;
            }
            continue;

        case core::render::RenderPumpDisposition::Rejected:
        case core::render::RenderPumpDisposition::Failed:
            last_failure_.store(result.failure, std::memory_order_relaxed);
            state_.store(WasapiRenderWorkerState::Failed, std::memory_order_release);
            revert_mmcss(mmcss_handle);
            return;
        }
    }

    state_.store(WasapiRenderWorkerState::Stopped, std::memory_order_release);
    revert_mmcss(mmcss_handle);
}

bool WasapiRenderWorkerRuntime::wait_for_device() noexcept {
    constexpr auto wait_slice = std::chrono::milliseconds{20};
    const auto result = renderer_.wait_until_writable(wait_slice);
    last_wait_result_.store(result, std::memory_order_relaxed);
    if (result == WasapiWaitResult::Ready) {
        return true;
    }
    if (result == WasapiWaitResult::Timeout) {
        saturating_increment(wait_timeout_count_);
        return true;
    }

    const auto renderer_snapshot = renderer_.snapshot();
    const auto failure =
        renderer_snapshot.last_failure == core::render::RenderFailure::None
        ? core::render::RenderFailure::BoundaryFailure
        : renderer_snapshot.last_failure;
    last_failure_.store(failure, std::memory_order_relaxed);
    last_platform_code_.store(
        static_cast<uint32_t>(renderer_.diagnostics().last_platform_code),
        std::memory_order_relaxed);
    state_.store(WasapiRenderWorkerState::Failed, std::memory_order_release);
    return false;
}

void WasapiRenderWorkerRuntime::revert_mmcss(HANDLE handle) noexcept {
    if (handle != nullptr) {
        static_cast<void>(AvRevertMmThreadCharacteristics(handle));
    }
}

} // namespace kivo::platform::windows::wasapi::detail
