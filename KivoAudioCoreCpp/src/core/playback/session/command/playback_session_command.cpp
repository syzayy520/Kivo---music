#include "../state/playback_session_state.hpp"

namespace kivo::core::playback {

namespace {

[[nodiscard]] bool requires_active_session(
    contract::CommandKind kind) noexcept {
    return kind != contract::CommandKind::OpenSource
        && kind != contract::CommandKind::Shutdown;
}

} // namespace

PlaybackCommandResult PlaybackSessionController::Impl::submit(
    const PlaybackCommand& command) noexcept {
    std::scoped_lock lock{mutex_};
    const auto previous = snapshot_.state;
    if (!command.is_valid()) {
        return reject(
            command.id,
            PlaybackCommandFailure::InvalidCommand,
            previous);
    }
    if (command.id.value <= snapshot_.last_command_id.value) {
        ++snapshot_.stale_commands;
        return reject(
            command.id,
            PlaybackCommandFailure::OutOfOrderCommand,
            previous);
    }
    snapshot_.last_command_id = command.id;

    if (snapshot_.state == contract::CoreState::Closed) {
        if (command.kind == contract::CommandKind::Shutdown) {
            return accept(command.id, previous);
        }
        return reject(
            command.id,
            PlaybackCommandFailure::SessionClosed,
            previous);
    }

    if (command.kind == contract::CommandKind::OpenSource) {
        if (command.generation.generation == 0
            || command.generation.generation
                <= snapshot_.session_generation) {
            ++snapshot_.stale_commands;
            return reject(
                command.id,
                PlaybackCommandFailure::StaleSession,
                previous);
        }
        const bool replacing = snapshot_.session_generation != 0;
        if (replacing
            && snapshot_.state == contract::CoreState::Draining) {
            saturating_increment(snapshot_.drain_cancellations);
        }
        snapshot_.session_generation = command.generation.generation;
        snapshot_.state = contract::CoreState::Ready;
        snapshot_.rendered_position = 0;
        snapshot_.paused_position = 0;
        snapshot_.pending_seek_target =
            contract::kInvalidSamplePosition;
        snapshot_.seek_generation = 0;
        snapshot_.flush_generation = 0;
        snapshot_.last_error_domain =
            contract::ErrorDomain::Unknown;
        snapshot_.recovery_action =
            contract::RecoveryAction::None;
        snapshot_.last_failure = PlaybackCommandFailure::None;
        if (replacing) {
            ++snapshot_.source_replacements;
        }
        ++snapshot_.accepted_commands;
        return {
            command.id,
            replacing
                ? contract::CommandDecision::Replace
                : contract::CommandDecision::Accept,
            PlaybackCommandFailure::None,
            previous,
            snapshot_.state
        };
    }

    if (requires_active_session(command.kind)
        && (snapshot_.session_generation == 0
            || command.generation.generation
                != snapshot_.session_generation)) {
        ++snapshot_.stale_commands;
        return reject(
            command.id,
            PlaybackCommandFailure::StaleSession,
            previous);
    }

    switch (command.kind) {
    case contract::CommandKind::CloseSource:
        if (snapshot_.state == contract::CoreState::Draining) {
            saturating_increment(snapshot_.drain_cancellations);
        }
        snapshot_.state = contract::CoreState::Idle;
        snapshot_.session_generation = 0;
        snapshot_.rendered_position = 0;
        snapshot_.paused_position = 0;
        snapshot_.pending_seek_target =
            contract::kInvalidSamplePosition;
        snapshot_.last_error_domain =
            contract::ErrorDomain::Unknown;
        snapshot_.recovery_action =
            contract::RecoveryAction::None;
        return accept(command.id, previous);
    case contract::CommandKind::Resume:
        if (snapshot_.state == contract::CoreState::Ready
            || snapshot_.state == contract::CoreState::Paused
            || snapshot_.state == contract::CoreState::Playing) {
            snapshot_.state = contract::CoreState::Playing;
            return accept(command.id, previous);
        }
        break;
    case contract::CommandKind::Pause:
        if (snapshot_.state == contract::CoreState::Playing
            || snapshot_.state == contract::CoreState::Paused) {
            snapshot_.state = contract::CoreState::Paused;
            snapshot_.paused_position = snapshot_.rendered_position;
            return accept(command.id, previous);
        }
        break;
    case contract::CommandKind::Stop:
        if (snapshot_.state != contract::CoreState::Idle) {
            if (snapshot_.state == contract::CoreState::Draining) {
                saturating_increment(snapshot_.drain_cancellations);
            }
            snapshot_.state = contract::CoreState::Stopped;
            snapshot_.pending_seek_target =
                contract::kInvalidSamplePosition;
            return accept(command.id, previous);
        }
        break;
    case contract::CommandKind::Seek:
        if (command.requested_position
                == contract::kInvalidSamplePosition
            || (snapshot_.state != contract::CoreState::Ready
                && snapshot_.state != contract::CoreState::Playing
                && snapshot_.state != contract::CoreState::Paused
                && snapshot_.state != contract::CoreState::Seeking)) {
            break;
        }
        if (snapshot_.state != contract::CoreState::Seeking) {
            state_before_seek_ = snapshot_.state;
        } else {
            ++snapshot_.coalesced_commands;
        }
        snapshot_.state = contract::CoreState::Seeking;
        snapshot_.pending_seek_target = command.requested_position;
        saturating_increment(snapshot_.seek_generation);
        ++snapshot_.accepted_commands;
        snapshot_.last_failure = PlaybackCommandFailure::None;
        return {
            command.id,
            previous == contract::CoreState::Seeking
                ? contract::CommandDecision::Coalesce
                : contract::CommandDecision::Accept,
            PlaybackCommandFailure::None,
            previous,
            snapshot_.state
        };
    case contract::CommandKind::Flush:
        if (snapshot_.state == contract::CoreState::Ready
            || snapshot_.state == contract::CoreState::Playing
            || snapshot_.state == contract::CoreState::Paused
            || snapshot_.state == contract::CoreState::Seeking) {
            saturating_increment(snapshot_.flush_generation);
            return accept(command.id, previous);
        }
        break;
    case contract::CommandKind::Shutdown:
        if (snapshot_.session_generation != 0
            && command.generation.generation
                != snapshot_.session_generation) {
            ++snapshot_.stale_commands;
            return reject(
                command.id,
                PlaybackCommandFailure::StaleSession,
                previous);
        }
        if (snapshot_.state == contract::CoreState::Draining) {
            saturating_increment(snapshot_.drain_cancellations);
        }
        snapshot_.state = contract::CoreState::Closed;
        snapshot_.session_generation = 0;
        snapshot_.rendered_position = 0;
        snapshot_.paused_position = 0;
        snapshot_.pending_seek_target =
            contract::kInvalidSamplePosition;
        snapshot_.last_error_domain =
            contract::ErrorDomain::Unknown;
        snapshot_.recovery_action =
            contract::RecoveryAction::None;
        return accept(command.id, previous);
    case contract::CommandKind::OpenSource:
        break;
    default:
        return reject(
            command.id,
            PlaybackCommandFailure::UnsupportedCommand,
            previous);
    }
    return reject(
        command.id,
        PlaybackCommandFailure::IllegalState,
        previous);
}

} // namespace kivo::core::playback
