#include "kivo/core/playback/recovery/executor/playback_recovery_executor.hpp"
#include "kivo/core/playback/session/controller/playback_session_controller.hpp"

#include "../fixture/scripted_recovery_operations.hpp"
#include "../fixture/session_test_assert.hpp"
#include "../fixture/session_test_factory.hpp"
#include "../runner/playback_session_test_cases.hpp"

namespace playback_session_test {

void recovery_executor_primary_success_restores_playback() {
    using namespace kivo;
    using core::contract::ErrorDomain;
    using core::contract::RecoveryAction;
    using core::playback::PlaybackRecoveryCompletion;
    core::playback::PlaybackSessionController session;
    SESSION_ASSERT(session.submit(command(
        1, core::contract::CommandKind::OpenSource, 10)).accepted());
    SESSION_ASSERT(session.submit(command(
        2, core::contract::CommandKind::Resume, 10)).accepted());
    ScriptedRecoveryOperations operations;
    operations.primary_action = RecoveryAction::ReopenDevice;
    operations.primary_completion = PlaybackRecoveryCompletion::Restored;
    core::playback::PlaybackRecoveryExecutor executor(session, operations);

    const auto result = executor.recover(10, ErrorDomain::DeviceLost);
    SESSION_ASSERT(result.accepted);
    SESSION_ASSERT(result.succeeded);
    SESSION_ASSERT(!result.fallback_attempted);
    SESSION_ASSERT(operations.execution_count == 1);
    SESSION_ASSERT(
        operations.observed_actions[0] == RecoveryAction::ReopenDevice);
    SESSION_ASSERT(
        session.snapshot().state == core::contract::CoreState::Playing);
}

void recovery_executor_fallback_can_stop_safely() {
    using namespace kivo;
    using core::contract::ErrorDomain;
    using core::contract::RecoveryAction;
    using core::playback::PlaybackRecoveryCompletion;
    core::playback::PlaybackSessionController session;
    SESSION_ASSERT(session.submit(command(
        1, core::contract::CommandKind::OpenSource, 10)).accepted());
    SESSION_ASSERT(session.submit(command(
        2, core::contract::CommandKind::Resume, 10)).accepted());
    ScriptedRecoveryOperations operations;
    operations.primary_action = RecoveryAction::RebuildSource;
    operations.primary_completion = PlaybackRecoveryCompletion::Failed;
    operations.fallback_action = RecoveryAction::StopMedia;
    operations.fallback_completion = PlaybackRecoveryCompletion::Stopped;
    core::playback::PlaybackRecoveryExecutor executor(session, operations);

    const auto result = executor.recover(
        10,
        ErrorDomain::SourceUnavailable);
    SESSION_ASSERT(result.accepted);
    SESSION_ASSERT(result.succeeded);
    SESSION_ASSERT(result.fallback_attempted);
    SESSION_ASSERT(operations.execution_count == 2);
    SESSION_ASSERT(
        operations.observed_actions[1] == RecoveryAction::StopMedia);
    SESSION_ASSERT(
        session.snapshot().state == core::contract::CoreState::Stopped);
}

void recovery_executor_double_failure_enters_failed() {
    using namespace kivo;
    using core::contract::ErrorDomain;
    using core::contract::RecoveryAction;
    using core::playback::PlaybackRecoveryCompletion;
    core::playback::PlaybackSessionController session;
    SESSION_ASSERT(session.submit(command(
        1, core::contract::CommandKind::OpenSource, 10)).accepted());
    ScriptedRecoveryOperations operations;
    operations.primary_action = RecoveryAction::ReconfigureFormat;
    operations.primary_completion = PlaybackRecoveryCompletion::Failed;
    operations.fallback_action = RecoveryAction::ReopenDevice;
    operations.fallback_completion = PlaybackRecoveryCompletion::Failed;
    core::playback::PlaybackRecoveryExecutor executor(session, operations);

    const auto result = executor.recover(10, ErrorDomain::FormatChanged);
    SESSION_ASSERT(result.accepted);
    SESSION_ASSERT(!result.succeeded);
    SESSION_ASSERT(result.fallback_attempted);
    SESSION_ASSERT(operations.execution_count == 2);
    SESSION_ASSERT(
        session.snapshot().state == core::contract::CoreState::Failed);
}

void recovery_executor_rejections_execute_nothing() {
    using namespace kivo;
    using core::contract::ErrorDomain;
    core::playback::PlaybackSessionController session;
    SESSION_ASSERT(session.submit(command(
        1, core::contract::CommandKind::OpenSource, 10)).accepted());
    ScriptedRecoveryOperations operations;
    core::playback::PlaybackRecoveryExecutor executor(session, operations);

    SESSION_ASSERT(
        !executor.recover(9, ErrorDomain::DeviceLost).accepted);
    SESSION_ASSERT(
        !executor.recover(
            10,
            ErrorDomain::UnsupportedFormat).accepted);
    SESSION_ASSERT(operations.execution_count == 0);
    SESSION_ASSERT(
        session.snapshot().state == core::contract::CoreState::Ready);
}

} // namespace playback_session_test
