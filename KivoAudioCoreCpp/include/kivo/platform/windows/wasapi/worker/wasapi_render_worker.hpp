#pragma once

#include <chrono>
#include <memory>

#include "kivo/core/render/queue/spsc_audio_block_queue.hpp"
#include "kivo/platform/windows/wasapi/renderer/wasapi_renderer.hpp"
#include "kivo/platform/windows/wasapi/worker/wasapi_render_worker_snapshot.hpp"

namespace kivo::platform::windows::wasapi {

namespace detail {
class WasapiRenderWorkerRuntime;
}

class WasapiRenderWorker final {
public:
    WasapiRenderWorker(
        WasapiRenderer& renderer,
        core::render::SpscAudioBlockQueue& queue);
    ~WasapiRenderWorker();

    WasapiRenderWorker(const WasapiRenderWorker&) = delete;
    WasapiRenderWorker& operator=(const WasapiRenderWorker&) = delete;
    WasapiRenderWorker(WasapiRenderWorker&&) = delete;
    WasapiRenderWorker& operator=(WasapiRenderWorker&&) = delete;

    [[nodiscard]] bool start() noexcept;
    void request_stop() noexcept;
    [[nodiscard]] bool join(std::chrono::milliseconds timeout) noexcept;
    [[nodiscard]] WasapiRenderWorkerSnapshot snapshot() const noexcept;

private:
    std::unique_ptr<detail::WasapiRenderWorkerRuntime> impl_;
};

} // namespace kivo::platform::windows::wasapi
