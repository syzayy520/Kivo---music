#include "kivo/platform/windows/wasapi/worker/wasapi_render_worker.hpp"

#include "platform/windows/wasapi/worker/wasapi_render_worker_runtime.hpp"

namespace kivo::platform::windows::wasapi {

WasapiRenderWorker::WasapiRenderWorker(
    WasapiRenderer& renderer,
    core::render::SpscAudioBlockQueue& queue)
    : impl_(std::make_unique<detail::WasapiRenderWorkerRuntime>(renderer, queue)) {}

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
