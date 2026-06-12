#pragma once

#include <Windows.h>

#include <atomic>

#include "kivo/platform/windows/wasapi/worker/wasapi_render_worker.hpp"

namespace kivo::platform::windows::wasapi::detail {

class WasapiRenderWorkerRuntime final {
public:
    WasapiRenderWorkerRuntime(
        WasapiRenderer& renderer,
        core::render::SpscAudioBlockQueue& queue) noexcept;
    ~WasapiRenderWorkerRuntime();

    [[nodiscard]] bool start() noexcept;
    void request_stop() noexcept;
    [[nodiscard]] bool join(std::chrono::milliseconds timeout) noexcept;
    [[nodiscard]] WasapiRenderWorkerSnapshot snapshot() const noexcept;

private:
    static DWORD WINAPI thread_entry(void* context) noexcept;
    void run() noexcept;
    [[nodiscard]] bool wait_for_device() noexcept;
    static void revert_mmcss(HANDLE handle) noexcept;
    void join_forever() noexcept;

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

} // namespace kivo::platform::windows::wasapi::detail
