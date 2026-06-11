#include <array>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <thread>
#include <vector>

#include "kivo/core/playback/recovery/playback_recovery_executor.hpp"
#include "kivo/core/playback/session/playback_session_controller.hpp"

namespace {

using namespace kivo;

#define SESSION_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            throw std::runtime_error("Assertion failed: " #condition); \
        } \
    } while (false)

[[nodiscard]] core::playback::PlaybackCommand command(
    uint64_t id,
    core::contract::CommandKind kind,
    uint64_t generation,
    core::contract::SamplePosition position =
        core::contract::kInvalidSamplePosition) noexcept {
    return {
        {id},
        kind,
        {generation, core::contract::CommandOrigin::User},
        position
    };
}

class ScriptedRecoveryOperations final
    : public core::playback::PlaybackRecoveryOperations {
public:
    core::contract::RecoveryAction primary_action{
        core::contract::RecoveryAction::None};
    core::playback::PlaybackRecoveryCompletion primary_completion{
        core::playback::PlaybackRecoveryCompletion::Failed};
    core::contract::RecoveryAction fallback_action{
        core::contract::RecoveryAction::None};
    core::playback::PlaybackRecoveryCompletion fallback_completion{
        core::playback::PlaybackRecoveryCompletion::Failed};
    std::array<core::contract::RecoveryAction, 2> observed_actions{};
    size_t execution_count{0};

    [[nodiscard]] core::playback::PlaybackRecoveryCompletion execute(
        core::contract::RecoveryAction action,
        uint64_t session_generation) noexcept override {
        if (session_generation == 0 || execution_count >= observed_actions.size()) {
            return core::playback::PlaybackRecoveryCompletion::Failed;
        }
        observed_actions[execution_count++] = action;
        if (action == primary_action) {
            return primary_completion;
        }
        if (action == fallback_action) {
            return fallback_completion;
        }
        return core::playback::PlaybackRecoveryCompletion::Failed;
    }
};

void lifecycle_and_pause_freeze() {
    core::playback::PlaybackSessionController session;
    SESSION_ASSERT(session.snapshot().state == core::contract::CoreState::Idle);
    SESSION_ASSERT(session.submit(command(
        1, core::contract::CommandKind::OpenSource, 10)).accepted());
    SESSION_ASSERT(session.submit(command(
        2, core::contract::CommandKind::Resume, 10)).accepted());
    SESSION_ASSERT(session.report_rendered_position(48000, 10));
    SESSION_ASSERT(session.submit(command(
        3, core::contract::CommandKind::Pause, 10)).accepted());
    const auto paused = session.snapshot();
    SESSION_ASSERT(paused.state == core::contract::CoreState::Paused);
    SESSION_ASSERT(paused.paused_position == 48000);
    SESSION_ASSERT(!session.report_rendered_position(49000, 10));
    SESSION_ASSERT(session.snapshot().paused_position == 48000);
    SESSION_ASSERT(session.submit(command(
        4, core::contract::CommandKind::Resume, 10)).accepted());
    SESSION_ASSERT(session.report_rendered_position(50000, 10));
}

void replacement_rejects_old_session_commands() {
    core::playback::PlaybackSessionController session;
    SESSION_ASSERT(session.submit(command(
        1, core::contract::CommandKind::OpenSource, 10)).accepted());
    const auto replaced = session.submit(command(
        2, core::contract::CommandKind::OpenSource, 11));
    SESSION_ASSERT(replaced.decision == core::contract::CommandDecision::Replace);
    const auto stale = session.submit(command(
        3, core::contract::CommandKind::Pause, 10));
    SESSION_ASSERT(!stale.accepted());
    SESSION_ASSERT(
        stale.failure == core::playback::PlaybackCommandFailure::StaleSession);
    SESSION_ASSERT(session.snapshot().session_generation == 11);
    SESSION_ASSERT(session.snapshot().source_replacements == 1);
}

void command_ids_are_globally_monotonic() {
    core::playback::PlaybackSessionController session;
    SESSION_ASSERT(session.submit(command(
        10, core::contract::CommandKind::OpenSource, 10)).accepted());
    const auto stale = session.submit(command(
        9, core::contract::CommandKind::Resume, 10));
    SESSION_ASSERT(!stale.accepted());
    SESSION_ASSERT(
        stale.failure
        == core::playback::PlaybackCommandFailure::OutOfOrderCommand);
    SESSION_ASSERT(session.snapshot().last_command_id.value == 10);
}

void failed_session_can_close_and_shutdown() {
    core::playback::PlaybackSessionController session;
    SESSION_ASSERT(session.submit(command(
        1, core::contract::CommandKind::OpenSource, 10)).accepted());
    SESSION_ASSERT(session.report_failure(10));
    SESSION_ASSERT(session.snapshot().state == core::contract::CoreState::Failed);
    SESSION_ASSERT(session.submit(command(
        2, core::contract::CommandKind::CloseSource, 10)).accepted());
    SESSION_ASSERT(session.snapshot().state == core::contract::CoreState::Idle);
    SESSION_ASSERT(session.submit(command(
        3, core::contract::CommandKind::Shutdown, 0)).accepted());
    SESSION_ASSERT(session.snapshot().state == core::contract::CoreState::Closed);
    const auto rejected = session.submit(command(
        4, core::contract::CommandKind::OpenSource, 11));
    SESSION_ASSERT(
        rejected.failure
        == core::playback::PlaybackCommandFailure::SessionClosed);
}

void concurrent_intake_remains_serialized() {
    core::playback::PlaybackSessionController session;
    SESSION_ASSERT(session.submit(command(
        1, core::contract::CommandKind::OpenSource, 10)).accepted());
    SESSION_ASSERT(session.submit(command(
        2, core::contract::CommandKind::Resume, 10)).accepted());

    std::vector<std::thread> threads;
    for (uint64_t id = 3; id <= 102; ++id) {
        threads.emplace_back([&session, id]() {
            const auto kind = (id % 2) == 0
                ? core::contract::CommandKind::Resume
                : core::contract::CommandKind::Pause;
            static_cast<void>(session.submit(command(id, kind, 10)));
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }
    const auto snapshot = session.snapshot();
    SESSION_ASSERT(snapshot.last_command_id.value == 102);
    SESSION_ASSERT(snapshot.accepted_commands + snapshot.rejected_commands == 102);
    SESSION_ASSERT(
        snapshot.state == core::contract::CoreState::Playing
        || snapshot.state == core::contract::CoreState::Paused);
}

void rapid_seek_coalesces_and_restores_state() {
    core::playback::PlaybackSessionController session;
    SESSION_ASSERT(session.submit(command(
        1, core::contract::CommandKind::OpenSource, 10)).accepted());
    SESSION_ASSERT(session.submit(command(
        2, core::contract::CommandKind::Resume, 10)).accepted());
    SESSION_ASSERT(session.report_rendered_position(12000, 10));

    const auto first = session.submit(command(
        3, core::contract::CommandKind::Seek, 10, 24000));
    const auto second = session.submit(command(
        4, core::contract::CommandKind::Seek, 10, 36000));
    SESSION_ASSERT(first.decision == core::contract::CommandDecision::Accept);
    SESSION_ASSERT(second.decision == core::contract::CommandDecision::Coalesce);
    auto snapshot = session.snapshot();
    SESSION_ASSERT(snapshot.state == core::contract::CoreState::Seeking);
    SESSION_ASSERT(snapshot.pending_seek_target == 36000);
    SESSION_ASSERT(snapshot.seek_generation == 2);
    SESSION_ASSERT(snapshot.coalesced_commands == 1);
    SESSION_ASSERT(session.complete_seek(10, true));
    snapshot = session.snapshot();
    SESSION_ASSERT(snapshot.state == core::contract::CoreState::Playing);
    SESSION_ASSERT(snapshot.rendered_position == 36000);
}

void paused_seek_stays_paused_and_flush_advances() {
    core::playback::PlaybackSessionController session;
    SESSION_ASSERT(session.submit(command(
        1, core::contract::CommandKind::OpenSource, 10)).accepted());
    SESSION_ASSERT(session.submit(command(
        2, core::contract::CommandKind::Resume, 10)).accepted());
    SESSION_ASSERT(session.report_rendered_position(12000, 10));
    SESSION_ASSERT(session.submit(command(
        3, core::contract::CommandKind::Pause, 10)).accepted());
    SESSION_ASSERT(session.submit(command(
        4, core::contract::CommandKind::Seek, 10, 6000)).accepted());
    SESSION_ASSERT(session.complete_seek(10, true));
    auto snapshot = session.snapshot();
    SESSION_ASSERT(snapshot.state == core::contract::CoreState::Paused);
    SESSION_ASSERT(snapshot.rendered_position == 6000);
    SESSION_ASSERT(snapshot.paused_position == 6000);
    SESSION_ASSERT(session.submit(command(
        5, core::contract::CommandKind::Flush, 10)).accepted());
    snapshot = session.snapshot();
    SESSION_ASSERT(snapshot.flush_generation == 1);
}

void recovery_policy_is_classified_and_generation_scoped() {
    using core::contract::ErrorDomain;
    using core::contract::RecoveryAction;
    constexpr auto device =
        core::playback::classify_recovery(ErrorDomain::DeviceLost);
    constexpr auto source =
        core::playback::classify_recovery(ErrorDomain::SourceUnavailable);
    constexpr auto unsupported =
        core::playback::classify_recovery(ErrorDomain::UnsupportedFormat);
    static_assert(device.recoverable);
    static_assert(device.primary == RecoveryAction::ReopenDevice);
    static_assert(source.primary == RecoveryAction::RebuildSource);
    static_assert(!unsupported.recoverable);

    core::playback::PlaybackSessionController session;
    SESSION_ASSERT(session.submit(command(
        1, core::contract::CommandKind::OpenSource, 10)).accepted());
    SESSION_ASSERT(session.submit(command(
        2, core::contract::CommandKind::Resume, 10)).accepted());
    const auto stale_decision =
        session.begin_recovery(9, ErrorDomain::DeviceLost);
    SESSION_ASSERT(!stale_decision.recoverable);
    SESSION_ASSERT(
        session.snapshot().state == core::contract::CoreState::Playing);
    const auto decision =
        session.begin_recovery(10, ErrorDomain::DeviceLost);
    SESSION_ASSERT(decision.recoverable);
    SESSION_ASSERT(
        session.snapshot().state == core::contract::CoreState::Recovering);
    SESSION_ASSERT(!session.complete_recovery(9, true));
    SESSION_ASSERT(session.complete_recovery(10, true));
    auto snapshot = session.snapshot();
    SESSION_ASSERT(snapshot.state == core::contract::CoreState::Playing);
    SESSION_ASSERT(snapshot.recovery_attempts == 1);
    SESSION_ASSERT(snapshot.recovery_successes == 1);

    static_cast<void>(
        session.begin_recovery(10, ErrorDomain::SourceUnavailable));
    SESSION_ASSERT(session.complete_recovery(10, false));
    snapshot = session.snapshot();
    SESSION_ASSERT(snapshot.state == core::contract::CoreState::Failed);
    SESSION_ASSERT(snapshot.recovery_failures == 1);
}

void recovery_interrupts_seek_without_restoring_stale_intent() {
    using core::contract::ErrorDomain;

    core::playback::PlaybackSessionController session;
    SESSION_ASSERT(session.submit(command(
        1, core::contract::CommandKind::OpenSource, 10)).accepted());
    SESSION_ASSERT(session.submit(command(
        2, core::contract::CommandKind::Resume, 10)).accepted());
    SESSION_ASSERT(session.submit(command(
        3, core::contract::CommandKind::Seek, 10, 48000)).accepted());
    SESSION_ASSERT(
        session.snapshot().state == core::contract::CoreState::Seeking);

    const auto decision =
        session.begin_recovery(10, ErrorDomain::DeviceLost);
    SESSION_ASSERT(decision.recoverable);
    auto snapshot = session.snapshot();
    SESSION_ASSERT(snapshot.state == core::contract::CoreState::Recovering);
    SESSION_ASSERT(
        snapshot.pending_seek_target
        == core::contract::kInvalidSamplePosition);
    SESSION_ASSERT(!session.complete_seek(10, true));
    SESSION_ASSERT(session.complete_recovery(10, true));
    snapshot = session.snapshot();
    SESSION_ASSERT(snapshot.state == core::contract::CoreState::Ready);
}

void new_source_clears_previous_error_identity() {
    using core::contract::ErrorDomain;
    using core::contract::RecoveryAction;

    core::playback::PlaybackSessionController session;
    SESSION_ASSERT(session.submit(command(
        1, core::contract::CommandKind::OpenSource, 10)).accepted());
    static_cast<void>(
        session.begin_recovery(10, ErrorDomain::SourceUnavailable));
    SESSION_ASSERT(session.complete_recovery(10, false));
    SESSION_ASSERT(session.submit(command(
        2, core::contract::CommandKind::OpenSource, 11)).accepted());
    const auto snapshot = session.snapshot();
    SESSION_ASSERT(snapshot.last_error_domain == ErrorDomain::Unknown);
    SESSION_ASSERT(snapshot.recovery_action == RecoveryAction::None);
}

void recovery_executor_primary_success_restores_playback() {
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
    SESSION_ASSERT(session.snapshot().state == core::contract::CoreState::Playing);
}

void recovery_executor_fallback_can_stop_safely() {
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
    SESSION_ASSERT(session.snapshot().state == core::contract::CoreState::Stopped);
}

void recovery_executor_double_failure_enters_failed() {
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
    SESSION_ASSERT(session.snapshot().state == core::contract::CoreState::Failed);
}

void recovery_executor_rejections_execute_nothing() {
    using core::contract::ErrorDomain;

    core::playback::PlaybackSessionController session;
    SESSION_ASSERT(session.submit(command(
        1, core::contract::CommandKind::OpenSource, 10)).accepted());
    ScriptedRecoveryOperations operations;
    core::playback::PlaybackRecoveryExecutor executor(session, operations);

    SESSION_ASSERT(!executor.recover(9, ErrorDomain::DeviceLost).accepted);
    SESSION_ASSERT(
        !executor.recover(10, ErrorDomain::UnsupportedFormat).accepted);
    SESSION_ASSERT(operations.execution_count == 0);
    SESSION_ASSERT(session.snapshot().state == core::contract::CoreState::Ready);
}

} // namespace

int main() {
    struct Test {
        const char* name;
        void (*run)();
    };
    const Test tests[] = {
        {"lifecycle_and_pause_freeze", lifecycle_and_pause_freeze},
        {"replacement_rejects_old_session_commands",
         replacement_rejects_old_session_commands},
        {"command_ids_are_globally_monotonic",
         command_ids_are_globally_monotonic},
        {"failed_session_can_close_and_shutdown",
         failed_session_can_close_and_shutdown},
        {"concurrent_intake_remains_serialized",
         concurrent_intake_remains_serialized},
        {"rapid_seek_coalesces_and_restores_state",
         rapid_seek_coalesces_and_restores_state},
        {"paused_seek_stays_paused_and_flush_advances",
         paused_seek_stays_paused_and_flush_advances},
        {"recovery_policy_is_classified_and_generation_scoped",
         recovery_policy_is_classified_and_generation_scoped},
        {"recovery_interrupts_seek_without_restoring_stale_intent",
         recovery_interrupts_seek_without_restoring_stale_intent},
        {"new_source_clears_previous_error_identity",
         new_source_clears_previous_error_identity},
        {"recovery_executor_primary_success_restores_playback",
         recovery_executor_primary_success_restores_playback},
        {"recovery_executor_fallback_can_stop_safely",
         recovery_executor_fallback_can_stop_safely},
        {"recovery_executor_double_failure_enters_failed",
         recovery_executor_double_failure_enters_failed},
        {"recovery_executor_rejections_execute_nothing",
         recovery_executor_rejections_execute_nothing}
    };
    int passed = 0;
    std::cout << "=== Playback Session Tests ===\n\n";
    for (const auto& test : tests) {
        std::cout << "  " << test.name << "... ";
        try {
            test.run();
            ++passed;
            std::cout << "PASS\n";
        } catch (const std::exception& exception) {
            std::cout << "FAIL: " << exception.what() << "\n";
        }
    }
    std::cout << "\n=== " << passed << "/" << std::size(tests)
              << " passed ===\n";
    return passed == std::size(tests) ? 0 : 1;
}
