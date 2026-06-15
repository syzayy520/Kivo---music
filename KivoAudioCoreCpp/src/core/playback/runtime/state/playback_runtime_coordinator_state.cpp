#include "playback_runtime_coordinator_state.hpp"

namespace kivo::core::playback {

PlaybackRuntimeCoordinator::Impl::Impl(
    PlaybackSessionController& session,
    decode::AudioDecodeBoundary& decoder,
    render::RenderBoundary& renderer) noexcept
    : session_(session), decoder_(decoder), renderer_(renderer) {}

PlaybackRuntimeCoordinator::Impl::~Impl() {
    std::scoped_lock lock{mutex_};
    static_cast<void>(release_runtime());
}

} // namespace kivo::core::playback
