#include "../state/playback_session_state.hpp"

#include <limits>

namespace kivo::core::playback {

PlaybackDrainTicket PlaybackSessionController::Impl::begin_drain(
    uint64_t session_generation) noexcept {
    std::scoped_lock lock{mutex_};
    if (snapshot_.state != contract::CoreState::Playing
        || session_generation == 0
        || session_generation != snapshot_.session_generation) {
        return {};
    }

    if (snapshot_.drain_generation
        == std::numeric_limits<uint64_t>::max()) {
        return {};
    }
    ++snapshot_.drain_generation;
    saturating_increment(snapshot_.drain_attempts);
    snapshot_.state = contract::CoreState::Draining;
    return {
        snapshot_.session_generation,
        snapshot_.drain_generation
    };
}

bool PlaybackSessionController::Impl::complete_drain(
    PlaybackDrainTicket ticket,
    PlaybackDrainCompletion completion) noexcept {
    std::scoped_lock lock{mutex_};
    if (!ticket.is_valid()
        || completion == PlaybackDrainCompletion::None
        || snapshot_.state != contract::CoreState::Draining
        || ticket.session_generation != snapshot_.session_generation
        || ticket.drain_generation != snapshot_.drain_generation) {
        return false;
    }

    if (completion == PlaybackDrainCompletion::Succeeded) {
        snapshot_.state = contract::CoreState::Stopped;
        saturating_increment(snapshot_.drain_successes);
        return true;
    }

    snapshot_.state = contract::CoreState::Failed;
    saturating_increment(snapshot_.drain_failures);
    return true;
}

} // namespace kivo::core::playback
