#include "kivo/core/playback/session/playback_session_controller.hpp"

#include <limits>
#include <mutex>

namespace kivo::core::playback {

namespace {

[[nodiscard]] bool requires_active_session(
    contract::CommandKind kind) noexcept {
    return kind != contract::CommandKind::OpenSource
        && kind != contract::CommandKind::Shutdown;
}

} // namespace

class PlaybackSessionController::Impl {
public:
    [[nodiscard]] PlaybackCommandResult submit(
        const PlaybackCommand& command) noexcept {
        std::scoped_lock lock{mutex};
        const auto previous = snapshot.state;
        if (!command.is_valid()) {
            return reject(
                command.id,
                PlaybackCommandFailure::InvalidCommand,
                previous);
        }
        if (command.id.value <= snapshot.last_command_id.value) {
            ++snapshot.stale_commands;
            return reject(
                command.id,
                PlaybackCommandFailure::OutOfOrderCommand,
                previous);
        }
        snapshot.last_command_id = command.id;

        if (snapshot.state == contract::CoreState::Closed) {
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
                    <= snapshot.session_generation) {
                ++snapshot.stale_commands;
                return reject(
                    command.id,
                    PlaybackCommandFailure::StaleSession,
                    previous);
            }
            const bool replacing = snapshot.session_generation != 0;
            snapshot.session_generation = command.generation.generation;
            snapshot.state = contract::CoreState::Ready;
            snapshot.rendered_position = 0;
            snapshot.paused_position = 0;
            snapshot.pending_seek_target =
                contract::kInvalidSamplePosition;
            snapshot.seek_generation = 0;
            snapshot.flush_generation = 0;
            snapshot.last_error_domain =
                contract::ErrorDomain::Unknown;
            snapshot.recovery_action =
                contract::RecoveryAction::None;
            snapshot.last_failure = PlaybackCommandFailure::None;
            if (replacing) {
                ++snapshot.source_replacements;
            }
            ++snapshot.accepted_commands;
            return {
                command.id,
                replacing
                    ? contract::CommandDecision::Replace
                    : contract::CommandDecision::Accept,
                PlaybackCommandFailure::None,
                previous,
                snapshot.state
            };
        }

        if (requires_active_session(command.kind)
            && (snapshot.session_generation == 0
                || command.generation.generation
                    != snapshot.session_generation)) {
            ++snapshot.stale_commands;
            return reject(
                command.id,
                PlaybackCommandFailure::StaleSession,
                previous);
        }

        switch (command.kind) {
        case contract::CommandKind::CloseSource:
            snapshot.state = contract::CoreState::Idle;
            snapshot.session_generation = 0;
            snapshot.rendered_position = 0;
            snapshot.paused_position = 0;
            snapshot.pending_seek_target =
                contract::kInvalidSamplePosition;
            snapshot.last_error_domain =
                contract::ErrorDomain::Unknown;
            snapshot.recovery_action =
                contract::RecoveryAction::None;
            return accept(command.id, previous);
        case contract::CommandKind::Resume:
            if (snapshot.state == contract::CoreState::Ready
                || snapshot.state == contract::CoreState::Paused
                || snapshot.state == contract::CoreState::Playing) {
                snapshot.state = contract::CoreState::Playing;
                return accept(command.id, previous);
            }
            break;
        case contract::CommandKind::Pause:
            if (snapshot.state == contract::CoreState::Playing
                || snapshot.state == contract::CoreState::Paused) {
                snapshot.state = contract::CoreState::Paused;
                snapshot.paused_position = snapshot.rendered_position;
                return accept(command.id, previous);
            }
            break;
        case contract::CommandKind::Stop:
            if (snapshot.state != contract::CoreState::Idle) {
                snapshot.state = contract::CoreState::Stopped;
                snapshot.pending_seek_target =
                    contract::kInvalidSamplePosition;
                return accept(command.id, previous);
            }
            break;
        case contract::CommandKind::Seek:
            if (command.requested_position
                    == contract::kInvalidSamplePosition
                || (snapshot.state != contract::CoreState::Ready
                    && snapshot.state != contract::CoreState::Playing
                    && snapshot.state != contract::CoreState::Paused
                    && snapshot.state != contract::CoreState::Seeking)) {
                break;
            }
            if (snapshot.state != contract::CoreState::Seeking) {
                state_before_seek = snapshot.state;
            } else {
                ++snapshot.coalesced_commands;
            }
            snapshot.state = contract::CoreState::Seeking;
            snapshot.pending_seek_target = command.requested_position;
            saturating_increment(snapshot.seek_generation);
            ++snapshot.accepted_commands;
            snapshot.last_failure = PlaybackCommandFailure::None;
            return {
                command.id,
                previous == contract::CoreState::Seeking
                    ? contract::CommandDecision::Coalesce
                    : contract::CommandDecision::Accept,
                PlaybackCommandFailure::None,
                previous,
                snapshot.state
            };
        case contract::CommandKind::Flush:
            if (snapshot.state == contract::CoreState::Ready
                || snapshot.state == contract::CoreState::Playing
                || snapshot.state == contract::CoreState::Paused
                || snapshot.state == contract::CoreState::Seeking) {
                saturating_increment(snapshot.flush_generation);
                return accept(command.id, previous);
            }
            break;
        case contract::CommandKind::Shutdown:
            if (snapshot.session_generation != 0
                && command.generation.generation
                    != snapshot.session_generation) {
                ++snapshot.stale_commands;
                return reject(
                    command.id,
                    PlaybackCommandFailure::StaleSession,
                    previous);
            }
            snapshot.state = contract::CoreState::Closed;
            snapshot.session_generation = 0;
            snapshot.rendered_position = 0;
            snapshot.paused_position = 0;
            snapshot.pending_seek_target =
                contract::kInvalidSamplePosition;
            snapshot.last_error_domain =
                contract::ErrorDomain::Unknown;
            snapshot.recovery_action =
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

    [[nodiscard]] bool report_rendered_position(
        contract::SamplePosition position,
        uint64_t session_generation) noexcept {
        std::scoped_lock lock{mutex};
        if (snapshot.state != contract::CoreState::Playing
            || session_generation == 0
            || session_generation != snapshot.session_generation
            || position < snapshot.rendered_position
            || position == contract::kInvalidSamplePosition) {
            return false;
        }
        snapshot.rendered_position = position;
        return true;
    }

    [[nodiscard]] bool report_failure(
        uint64_t session_generation) noexcept {
        std::scoped_lock lock{mutex};
        if (session_generation == 0
            || session_generation != snapshot.session_generation
            || snapshot.state == contract::CoreState::Closed) {
            return false;
        }
        snapshot.state = contract::CoreState::Failed;
        snapshot.pending_seek_target = contract::kInvalidSamplePosition;
        return true;
    }

    [[nodiscard]] bool complete_seek(
        uint64_t session_generation,
        bool succeeded) noexcept {
        std::scoped_lock lock{mutex};
        if (snapshot.state != contract::CoreState::Seeking
            || session_generation == 0
            || session_generation != snapshot.session_generation
            || snapshot.pending_seek_target
                == contract::kInvalidSamplePosition) {
            return false;
        }
        if (!succeeded) {
            snapshot.state = contract::CoreState::Failed;
            snapshot.pending_seek_target =
                contract::kInvalidSamplePosition;
            return true;
        }
        snapshot.rendered_position = snapshot.pending_seek_target;
        if (state_before_seek == contract::CoreState::Paused) {
            snapshot.paused_position = snapshot.pending_seek_target;
        }
        snapshot.pending_seek_target = contract::kInvalidSamplePosition;
        snapshot.state = state_before_seek;
        return true;
    }

    [[nodiscard]] PlaybackRecoveryDecision begin_recovery(
        uint64_t session_generation,
        contract::ErrorDomain domain) noexcept {
        std::scoped_lock lock{mutex};
        const auto decision = classify_recovery(domain);
        if (!decision.is_valid()
            || !decision.recoverable
            || session_generation == 0
            || session_generation != snapshot.session_generation
            || snapshot.state == contract::CoreState::Closed
            || snapshot.state == contract::CoreState::Idle) {
            auto rejected = decision;
            rejected.recoverable = false;
            return rejected;
        }
        if (snapshot.state != contract::CoreState::Recovering) {
            if (snapshot.state == contract::CoreState::Seeking) {
                snapshot.pending_seek_target =
                    contract::kInvalidSamplePosition;
                state_before_recovery = contract::CoreState::Ready;
            } else {
                state_before_recovery = snapshot.state;
            }
        }
        snapshot.state = contract::CoreState::Recovering;
        snapshot.last_error_domain = domain;
        snapshot.recovery_action = decision.primary;
        saturating_increment(snapshot.recovery_attempts);
        return decision;
    }

    [[nodiscard]] bool complete_recovery(
        uint64_t session_generation,
        PlaybackRecoveryCompletion completion) noexcept {
        std::scoped_lock lock{mutex};
        if (snapshot.state != contract::CoreState::Recovering
            || session_generation == 0
            || session_generation != snapshot.session_generation) {
            return false;
        }
        snapshot.recovery_action = contract::RecoveryAction::None;
        if (completion == PlaybackRecoveryCompletion::Failed) {
            snapshot.state = contract::CoreState::Failed;
            saturating_increment(snapshot.recovery_failures);
            return true;
        }
        switch (completion) {
        case PlaybackRecoveryCompletion::Restored:
            snapshot.state =
                state_before_recovery == contract::CoreState::Playing
                    || state_before_recovery == contract::CoreState::Paused
                    || state_before_recovery == contract::CoreState::Ready
                ? state_before_recovery
                : contract::CoreState::Ready;
            break;
        case PlaybackRecoveryCompletion::Ready:
            snapshot.state = contract::CoreState::Ready;
            break;
        case PlaybackRecoveryCompletion::Stopped:
            snapshot.state = contract::CoreState::Stopped;
            break;
        case PlaybackRecoveryCompletion::Failed:
            break;
        }
        saturating_increment(snapshot.recovery_successes);
        return true;
    }

    [[nodiscard]] PlaybackSessionSnapshot read_snapshot() const noexcept {
        std::scoped_lock lock{mutex};
        return snapshot;
    }

private:
    static void saturating_increment(uint64_t& value) noexcept {
        if (value != std::numeric_limits<uint64_t>::max()) {
            ++value;
        }
    }

    [[nodiscard]] PlaybackCommandResult accept(
        contract::CommandId command_id,
        contract::CoreState previous) noexcept {
        ++snapshot.accepted_commands;
        snapshot.last_failure = PlaybackCommandFailure::None;
        return {
            command_id,
            contract::CommandDecision::Accept,
            PlaybackCommandFailure::None,
            previous,
            snapshot.state
        };
    }

    [[nodiscard]] PlaybackCommandResult reject(
        contract::CommandId command_id,
        PlaybackCommandFailure failure,
        contract::CoreState previous) noexcept {
        ++snapshot.rejected_commands;
        snapshot.last_failure = failure;
        return {
            command_id,
            contract::CommandDecision::Reject,
            failure,
            previous,
            snapshot.state
        };
    }

    mutable std::mutex mutex{};
    PlaybackSessionSnapshot snapshot{};
    contract::CoreState state_before_seek{contract::CoreState::Ready};
    contract::CoreState state_before_recovery{contract::CoreState::Ready};
};

PlaybackSessionController::PlaybackSessionController()
    : impl_(std::make_unique<Impl>()) {}

PlaybackSessionController::~PlaybackSessionController() = default;

PlaybackCommandResult PlaybackSessionController::submit(
    const PlaybackCommand& command) noexcept {
    return impl_
        ? impl_->submit(command)
        : PlaybackCommandResult{};
}

bool PlaybackSessionController::report_rendered_position(
    contract::SamplePosition position,
    uint64_t session_generation) noexcept {
    return impl_
        && impl_->report_rendered_position(position, session_generation);
}

bool PlaybackSessionController::report_failure(
    uint64_t session_generation) noexcept {
    return impl_ && impl_->report_failure(session_generation);
}

bool PlaybackSessionController::complete_seek(
    uint64_t session_generation,
    bool succeeded) noexcept {
    return impl_
        && impl_->complete_seek(session_generation, succeeded);
}

PlaybackRecoveryDecision PlaybackSessionController::begin_recovery(
    uint64_t session_generation,
    contract::ErrorDomain domain) noexcept {
    return impl_
        ? impl_->begin_recovery(session_generation, domain)
        : PlaybackRecoveryDecision{};
}

bool PlaybackSessionController::complete_recovery(
    uint64_t session_generation,
    bool succeeded) noexcept {
    return complete_recovery(
        session_generation,
        succeeded
            ? PlaybackRecoveryCompletion::Restored
            : PlaybackRecoveryCompletion::Failed);
}

bool PlaybackSessionController::complete_recovery(
    uint64_t session_generation,
    PlaybackRecoveryCompletion completion) noexcept {
    return impl_
        && impl_->complete_recovery(session_generation, completion);
}

PlaybackSessionSnapshot PlaybackSessionController::snapshot() const noexcept {
    return impl_ ? impl_->read_snapshot() : PlaybackSessionSnapshot{};
}

} // namespace kivo::core::playback
