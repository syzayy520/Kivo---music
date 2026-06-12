#include "../state/playback_session_state.hpp"

namespace kivo::core::playback {

bool PlaybackSessionController::Impl::report_rendered_position(
    contract::SamplePosition position,
    uint64_t session_generation) noexcept {
    std::scoped_lock lock{mutex_};
    if (snapshot_.state != contract::CoreState::Playing
        || session_generation == 0
        || session_generation != snapshot_.session_generation
        || position < snapshot_.rendered_position
        || position == contract::kInvalidSamplePosition) {
        return false;
    }
    snapshot_.rendered_position = position;
    return true;
}

bool PlaybackSessionController::Impl::report_failure(
    uint64_t session_generation) noexcept {
    std::scoped_lock lock{mutex_};
    if (session_generation == 0
        || session_generation != snapshot_.session_generation
        || snapshot_.state == contract::CoreState::Closed) {
        return false;
    }
    snapshot_.state = contract::CoreState::Failed;
    snapshot_.pending_seek_target =
        contract::kInvalidSamplePosition;
    return true;
}

} // namespace kivo::core::playback
