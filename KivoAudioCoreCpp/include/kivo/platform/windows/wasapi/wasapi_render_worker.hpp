#pragma once

#include <chrono>
#include <memory>

#include "kivo/core/render/queue/spsc_audio_block_queue.hpp"
#include "kivo/platform/windows/wasapi/wasapi_render_worker_snapshot.hpp"
#include "kivo/platform/windows/wasapi/wasapi_renderer.hpp"

namespace kivo::platform::windows::wasapi {

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
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace kivo::platform::windows::wasapi
