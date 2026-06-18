#include "kivo/core/playback/runtime/coordinator/playback_runtime_coordinator.hpp"

#include <utility>

#include "../result/playback_runtime_result_factory.hpp"
#include "../state/playback_runtime_coordinator_state.hpp"

namespace kivo::core::playback {

PlaybackRuntimeCoordinator::PlaybackRuntimeCoordinator(
    PlaybackSessionController& session,
    decode::AudioDecodeBoundary& decoder,
    render::RenderBoundary& renderer)
    : impl_(std::make_unique<Impl>(session, decoder, renderer)) {}

PlaybackRuntimeCoordinator::~PlaybackRuntimeCoordinator() = default;

PlaybackRuntimeResult PlaybackRuntimeCoordinator::open(
    const PlaybackCommand& command,
    std::unique_ptr<decode::ByteSourceBoundary> source,
    const PlaybackRuntimeOpenRequest& request) noexcept {
    return impl_
        ? impl_->open(command, std::move(source), request)
        : runtime_result::failed(PlaybackRuntimeFailure::BoundaryFailure);
}

PlaybackRuntimeResult PlaybackRuntimeCoordinator::execute(
    const PlaybackCommand& command) noexcept {
    return impl_
        ? impl_->execute(command)
        : runtime_result::failed(PlaybackRuntimeFailure::BoundaryFailure);
}

PlaybackRuntimeResult PlaybackRuntimeCoordinator::seek(
    const PlaybackCommand& command) noexcept {
    return impl_
        ? impl_->seek(command)
        : runtime_result::failed(PlaybackRuntimeFailure::BoundaryFailure);
}

PlaybackRuntimeResult PlaybackRuntimeCoordinator::set_volume(
    const PlaybackCommand& command,
    double gain) noexcept {
    return impl_
        ? impl_->set_volume(command, gain)
        : runtime_result::failed(PlaybackRuntimeFailure::BoundaryFailure);
}

PlaybackRuntimeResult PlaybackRuntimeCoordinator::recover() noexcept {
    return impl_
        ? impl_->recover()
        : runtime_result::failed(PlaybackRuntimeFailure::BoundaryFailure);
}

DecodeRenderQueueProducerResult
PlaybackRuntimeCoordinator::produce_step() noexcept {
    return impl_
        ? impl_->produce_step()
        : DecodeRenderQueueProducerResult{};
}

render::RenderPumpResult PlaybackRuntimeCoordinator::render_step() noexcept {
    return impl_
        ? impl_->render_step()
        : render::RenderPumpResult{
            render::RenderPumpDisposition::Failed,
            render::RenderFailure::BoundaryFailure};
}

PlaybackRuntimeSnapshot PlaybackRuntimeCoordinator::snapshot() const noexcept {
    return impl_ ? impl_->snapshot() : PlaybackRuntimeSnapshot{};
}

} // namespace kivo::core::playback
