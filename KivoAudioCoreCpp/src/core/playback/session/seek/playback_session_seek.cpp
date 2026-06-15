#include "../state/playback_session_state.hpp"

namespace kivo::core::playback {

bool PlaybackSessionController::Impl::complete_seek(
    uint64_t session_generation,
    bool succeeded) noexcept {
    std::scoped_lock lock{mutex_};
    if (snapshot_.state != contract::CoreState::Seeking
        || session_generation == 0
        || session_generation != snapshot_.session_generation
        || snapshot_.pending_seek_target
            == contract::kInvalidSamplePosition) {
        return false;
    }
    if (!succeeded) {
        snapshot_.state = contract::CoreState::Failed;
        snapshot_.pending_seek_target =
            contract::kInvalidSamplePosition;
        return true;
    }
    snapshot_.rendered_position = snapshot_.pending_seek_target;
    if (state_before_seek_ == contract::CoreState::Paused) {
        snapshot_.paused_position = snapshot_.pending_seek_target;
    }
    snapshot_.pending_seek_target =
        contract::kInvalidSamplePosition;
    snapshot_.state = state_before_seek_;
    return true;
}

} // namespace kivo::core::playback
