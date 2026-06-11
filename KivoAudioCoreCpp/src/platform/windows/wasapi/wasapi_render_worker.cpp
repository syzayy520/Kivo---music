#include "kivo/platform/windows/wasapi/wasapi_render_worker.hpp"

#include <Windows.h>
#include <avrt.h>

#include <algorithm>
#include <atomic>
#include <limits>
#include <new>

#include "kivo/core/render/pump/render_pump.hpp"

namespace kivo::platform::windows::wasapi {

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

[[nodiscard]] DWORD wait_milliseconds(std::chrono::milliseconds timeout) noexcept {
    if (timeout.count() <= 0) {
        return 0;
    }
    const auto maximum = static_cast<int64_t>(INFINITE - 1);
    return static_cast<DWORD>(std::min(timeout.count(), maximum));
}

} // namespace

class WasapiRenderWorker::Impl {
public:
    Impl(
        WasapiRenderer& renderer,
        core::render::SpscAudioBlockQueue& queue) noexcept
        : renderer_(renderer), queue_(queue) {}

    ~Impl() {
        request_stop();
        join_forever();
    }

    [[nodiscard]] bool start() noexcept {
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
            &Impl::thread_entry,
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

    void request_stop() noexcept {
        stop_requested_.store(true, std::memory_order_release);
        auto expected = WasapiRenderWorkerState::Running;
        static_cast<void>(state_.compare_exchange_strong(
            expected,
            WasapiRenderWorkerState::Stopping,
            std::memory_order_acq_rel,
            std::memory_order_acquire));
    }

    [[nodiscard]] bool join(std::chrono::milliseconds timeout) noexcept {
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

    [[nodiscard]] WasapiRenderWorkerSnapshot snapshot() const noexcept {
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

private:
    static DWORD WINAPI thread_entry(void* context) noexcept {
        static_cast<Impl*>(context)->run();
        return 0;
    }

    void run() noexcept {
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

    [[nodiscard]] bool wait_for_device() noexcept {
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
        const auto failure = renderer_snapshot.last_failure == core::render::RenderFailure::None
            ? core::render::RenderFailure::BoundaryFailure
            : renderer_snapshot.last_failure;
        last_failure_.store(failure, std::memory_order_relaxed);
        last_platform_code_.store(
            static_cast<uint32_t>(renderer_.diagnostics().last_platform_code),
            std::memory_order_relaxed);
        state_.store(WasapiRenderWorkerState::Failed, std::memory_order_release);
        return false;
    }

    static void revert_mmcss(HANDLE handle) noexcept {
        if (handle != nullptr) {
            static_cast<void>(AvRevertMmThreadCharacteristics(handle));
        }
    }

    void join_forever() noexcept {
        if (thread_handle_ == nullptr) {
            return;
        }
        static_cast<void>(WaitForSingleObject(thread_handle_, INFINITE));
        CloseHandle(thread_handle_);
        thread_handle_ = nullptr;
    }

    WasapiRenderer& renderer_;
    core::render::SpscAudioBlockQueue& queue_;
    HANDLE thread_handle_{nullptr};
    std::atomic<bool> stop_requested_{false};
    std::atomic<WasapiRenderWorkerState> state_{WasapiRenderWorkerState::Idle};
    std::atomic<core::render::RenderFailure> last_failure_{
        core::render::RenderFailure::None};
    std::atomic<WasapiWaitResult> last_wait_result_{WasapiWaitResult::Closed};
    std::atomic<bool> mmcss_registered_{false};
    std::atomic<bool> mmcss_high_priority_{false};
    std::atomic<core::contract::FrameCount> accepted_frames_{0};
    std::atomic<uint64_t> discarded_stale_blocks_{0};
    std::atomic<uint64_t> wait_timeout_count_{0};
    std::atomic<uint32_t> last_platform_code_{0};
};

WasapiRenderWorker::WasapiRenderWorker(
    WasapiRenderer& renderer,
    core::render::SpscAudioBlockQueue& queue)
    : impl_(std::make_unique<Impl>(renderer, queue)) {}

WasapiRenderWorker::~WasapiRenderWorker() = default;

bool WasapiRenderWorker::start() noexcept {
    return impl_ && impl_->start();
}

void WasapiRenderWorker::request_stop() noexcept {
    if (impl_) {
        impl_->request_stop();
    }
}

bool WasapiRenderWorker::join(std::chrono::milliseconds timeout) noexcept {
    return impl_ && impl_->join(timeout);
}

WasapiRenderWorkerSnapshot WasapiRenderWorker::snapshot() const noexcept {
    return impl_ ? impl_->snapshot() : WasapiRenderWorkerSnapshot{};
}

} // namespace kivo::platform::windows::wasapi
