#include "../state/playback_session_state.hpp"

namespace kivo::core::playback {

PlaybackSessionSnapshot
PlaybackSessionController::Impl::read_snapshot() const noexcept {
    std::scoped_lock lock{mutex_};
    return snapshot_;
}

} // namespace kivo::core::playback
