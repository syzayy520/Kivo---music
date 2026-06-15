#include "../state/playback_session_state.hpp"

#include <limits>

namespace kivo::core::playback {

void PlaybackSessionController::Impl::saturating_increment(
    uint64_t& value) noexcept {
    if (value != std::numeric_limits<uint64_t>::max()) {
        ++value;
    }
}

PlaybackCommandResult PlaybackSessionController::Impl::accept(
    contract::CommandId command_id,
    contract::CoreState previous) noexcept {
    ++snapshot_.accepted_commands;
    snapshot_.last_failure = PlaybackCommandFailure::None;
    return {
        command_id,
        contract::CommandDecision::Accept,
        PlaybackCommandFailure::None,
        previous,
        snapshot_.state
    };
}

PlaybackCommandResult PlaybackSessionController::Impl::reject(
    contract::CommandId command_id,
    PlaybackCommandFailure failure,
    contract::CoreState previous) noexcept {
    ++snapshot_.rejected_commands;
    snapshot_.last_failure = failure;
    return {
        command_id,
        contract::CommandDecision::Reject,
        failure,
        previous,
        snapshot_.state
    };
}

} // namespace kivo::core::playback
