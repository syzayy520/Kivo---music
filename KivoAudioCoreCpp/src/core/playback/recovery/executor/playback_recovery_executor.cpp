#include "kivo/core/playback/recovery/executor/playback_recovery_executor.hpp"

#include "kivo/core/playback/session/controller/playback_session_controller.hpp"

namespace kivo::core::playback {

PlaybackRecoveryExecutor::PlaybackRecoveryExecutor(
    PlaybackSessionController& session,
    PlaybackRecoveryOperations& operations) noexcept
    : session_(session), operations_(operations) {}

PlaybackRecoveryExecutionResult PlaybackRecoveryExecutor::recover(
    uint64_t session_generation,
    contract::ErrorDomain domain) noexcept {
    const auto decision = session_.begin_recovery(
        session_generation,
        domain);
    PlaybackRecoveryExecutionResult result{
        decision.recoverable,
        false,
        false,
        decision.primary,
        decision.fallback,
        PlaybackRecoveryCompletion::Failed
    };
    if (!decision.recoverable) {
        return result;
    }

    result.completion = operations_.execute(
        decision.primary,
        session_generation);
    if (result.completion == PlaybackRecoveryCompletion::Failed
        && decision.fallback != contract::RecoveryAction::None) {
        result.fallback_attempted = true;
        result.completion = operations_.execute(
            decision.fallback,
            session_generation);
    }

    result.succeeded =
        result.completion != PlaybackRecoveryCompletion::Failed;
    if (!session_.complete_recovery(
            session_generation,
            result.completion)) {
        result.succeeded = false;
        result.completion = PlaybackRecoveryCompletion::Failed;
    }
    return result;
}

} // namespace kivo::core::playback
