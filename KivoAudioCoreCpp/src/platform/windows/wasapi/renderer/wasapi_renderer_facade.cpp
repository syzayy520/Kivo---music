#include "kivo/platform/windows/wasapi/renderer/wasapi_renderer.hpp"

#include "platform/windows/wasapi/renderer/wasapi_renderer_state.hpp"

namespace kivo::platform::windows::wasapi {

using core::render::RenderControlResult;
using core::render::RenderFailure;
using core::render::RenderLifecycleState;
using core::render::RenderWriteResult;

WasapiRenderer::WasapiRenderer()
    : impl_(std::make_unique<detail::WasapiRendererState>()) {}

WasapiRenderer::~WasapiRenderer() = default;

core::render::RendererIdentity WasapiRenderer::identity() const noexcept {
    return impl_ ? impl_->identity() : core::render::RendererIdentity{};
}

core::render::RendererCapabilitySnapshot WasapiRenderer::capabilities() const noexcept {
    return impl_ ? impl_->capabilities() : core::render::RendererCapabilitySnapshot{};
}

core::render::RenderOpenResult WasapiRenderer::open(
    const core::render::RenderOpenRequest& request) noexcept {
    return impl_
        ? impl_->open(request)
        : core::render::RenderOpenResult::Failed(RenderFailure::BoundaryFailure);
}

RenderControlResult WasapiRenderer::start() noexcept {
    return impl_
        ? impl_->start()
        : RenderControlResult::Failed(RenderFailure::BoundaryFailure);
}

RenderControlResult WasapiRenderer::flush(
    const core::render::RenderFlushRequest& request) noexcept {
    return impl_
        ? impl_->flush(request)
        : RenderControlResult::Failed(RenderFailure::BoundaryFailure);
}

RenderControlResult WasapiRenderer::drain(
    const core::render::RenderDrainRequest& request) noexcept {
    return impl_
        ? impl_->drain(request)
        : RenderControlResult::Failed(RenderFailure::BoundaryFailure);
}

RenderControlResult WasapiRenderer::reset(
    const core::render::RenderResetRequest& request) noexcept {
    return impl_
        ? impl_->reset(request)
        : RenderControlResult::Failed(RenderFailure::BoundaryFailure);
}

RenderControlResult WasapiRenderer::stop() noexcept {
    return impl_
        ? impl_->stop()
        : RenderControlResult::Failed(RenderFailure::BoundaryFailure);
}

RenderControlResult WasapiRenderer::close() noexcept {
    return impl_
        ? impl_->close()
        : RenderControlResult::Succeeded(RenderLifecycleState::Closed);
}

core::render::RenderSnapshot WasapiRenderer::snapshot() const noexcept {
    return impl_ ? impl_->snapshot() : core::render::RenderSnapshot{};
}

RenderWriteResult WasapiRenderer::write(
    const core::render::AudioBlockView& block) noexcept {
    return impl_
        ? impl_->write(block)
        : RenderWriteResult::Failed(block.frame_count, RenderFailure::BoundaryFailure);
}

WasapiWaitResult WasapiRenderer::wait_until_writable(
    std::chrono::milliseconds timeout) noexcept {
    return impl_ ? impl_->wait_until_writable(timeout) : WasapiWaitResult::Closed;
}

WasapiDiagnostics WasapiRenderer::diagnostics() const noexcept {
    return impl_ ? impl_->diagnostics() : WasapiDiagnostics{};
}

} // namespace kivo::platform::windows::wasapi
