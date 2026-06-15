#include "../state/playback_session_state.hpp"

namespace kivo::core::playback {

PlaybackRecoveryDecision
PlaybackSessionController::Impl::begin_recovery(
    uint64_t session_generation,
    contract::ErrorDomain domain) noexcept {
    std::scoped_lock lock{mutex_};
    const auto decision = classify_recovery(domain);
    if (!decision.is_valid()
        || !decision.recoverable
        || session_generation == 0
        || session_generation != snapshot_.session_generation
        || snapshot_.state == contract::CoreState::Closed
        || snapshot_.state == contract::CoreState::Idle) {
        auto rejected = decision;
        rejected.recoverable = false;
        return rejected;
    }
    if (snapshot_.state != contract::CoreState::Recovering) {
        if (snapshot_.state == contract::CoreState::Seeking) {
            snapshot_.pending_seek_target =
                contract::kInvalidSamplePosition;
            state_before_recovery_ = contract::CoreState::Ready;
        } else {
            state_before_recovery_ = snapshot_.state;
        }
    }
    snapshot_.state = contract::CoreState::Recovering;
    snapshot_.last_error_domain = domain;
    snapshot_.recovery_action = decision.primary;
    saturating_increment(snapshot_.recovery_attempts);
    return decision;
}

bool PlaybackSessionController::Impl::complete_recovery(
    uint64_t session_generation,
    PlaybackRecoveryCompletion completion) noexcept {
    std::scoped_lock lock{mutex_};
    if (snapshot_.state != contract::CoreState::Recovering
        || session_generation == 0
        || session_generation != snapshot_.session_generation) {
        return false;
    }
    snapshot_.recovery_action = contract::RecoveryAction::None;
    if (completion == PlaybackRecoveryCompletion::Failed) {
        snapshot_.state = contract::CoreState::Failed;
        saturating_increment(snapshot_.recovery_failures);
        return true;
    }
    switch (completion) {
    case PlaybackRecoveryCompletion::Restored:
        snapshot_.state =
            state_before_recovery_ == contract::CoreState::Playing
                || state_before_recovery_ == contract::CoreState::Paused
                || state_before_recovery_ == contract::CoreState::Ready
            ? state_before_recovery_
            : contract::CoreState::Ready;
        break;
    case PlaybackRecoveryCompletion::Ready:
        snapshot_.state = contract::CoreState::Ready;
        break;
    case PlaybackRecoveryCompletion::Stopped:
        snapshot_.state = contract::CoreState::Stopped;
        break;
    case PlaybackRecoveryCompletion::Failed:
        break;
    }
    saturating_increment(snapshot_.recovery_successes);
    return true;
}

} // namespace kivo::core::playback
