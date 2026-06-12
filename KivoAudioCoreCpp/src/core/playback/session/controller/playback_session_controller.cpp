#include "kivo/core/playback/session/controller/playback_session_controller.hpp"

#include "../state/playback_session_state.hpp"

namespace kivo::core::playback {

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
        && impl_->report_rendered_position(
            position,
            session_generation);
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

PlaybackDrainTicket PlaybackSessionController::begin_drain(
    uint64_t session_generation) noexcept {
    return impl_
        ? impl_->begin_drain(session_generation)
        : PlaybackDrainTicket{};
}

bool PlaybackSessionController::complete_drain(
    PlaybackDrainTicket ticket,
    PlaybackDrainCompletion completion) noexcept {
    return impl_
        && impl_->complete_drain(ticket, completion);
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
        && impl_->complete_recovery(
            session_generation,
            completion);
}

PlaybackSessionSnapshot
PlaybackSessionController::snapshot() const noexcept {
    return impl_
        ? impl_->read_snapshot()
        : PlaybackSessionSnapshot{};
}

} // namespace kivo::core::playback
