#include "../../contract_tests_main.h"
#include "kivo/core/contract/state/scenario/seeking_reentry_rule.hpp"
#include "kivo/core/contract/state/scenario/recovering_seek_rule.hpp"
#include "kivo/core/contract/state/scenario/draining_pause_rule.hpp"
#include "kivo/core/contract/state/scenario/failed_close_rule.hpp"
#include "kivo/core/contract/state/scenario/closed_mutation_rule.hpp"
#include "kivo/core/contract/state/scenario/device_lost_during_seek_rule.hpp"
#include "kivo/core/contract/state/scenario/shutdown_during_drain_rule.hpp"

using namespace kivo::core::contract;

namespace {

void seeking_reentry_rule_default_is_unknown(ContractTestRunner& runner) {
    runner.run("seeking_reentry_rule_default_is_unknown", []() {
        SeekingReentryRule r{};
        ASSERT(r == SeekingReentryRule::Unknown);
    });
}

void seeking_reentry_rule_merge_seek(ContractTestRunner& runner) {
    runner.run("seeking_reentry_rule_merge_seek", []() {
        ASSERT(static_cast<uint8_t>(SeekingReentryRule::MergeSeek) == 1);
    });
}

void seeking_reentry_rule_queue_seek(ContractTestRunner& runner) {
    runner.run("seeking_reentry_rule_queue_seek", []() {
        ASSERT(static_cast<uint8_t>(SeekingReentryRule::QueueSeek) == 2);
    });
}

void seeking_reentry_rule_replace_with_latest_seek(ContractTestRunner& runner) {
    runner.run("seeking_reentry_rule_replace_with_latest_seek", []() {
        ASSERT(static_cast<uint8_t>(SeekingReentryRule::ReplaceWithLatestSeek) == 3);
    });
}

void seeking_reentry_rule_reject_new_seek(ContractTestRunner& runner) {
    runner.run("seeking_reentry_rule_reject_new_seek", []() {
        ASSERT(static_cast<uint8_t>(SeekingReentryRule::RejectNewSeek) == 4);
    });
}

void recovering_seek_rule_default_is_unknown(ContractTestRunner& runner) {
    runner.run("recovering_seek_rule_default_is_unknown", []() {
        RecoveringSeekRule r{};
        ASSERT(r == RecoveringSeekRule::Unknown);
    });
}

void recovering_seek_rule_reject_seek(ContractTestRunner& runner) {
    runner.run("recovering_seek_rule_reject_seek", []() {
        ASSERT(static_cast<uint8_t>(RecoveringSeekRule::RejectSeek) == 1);
    });
}

void recovering_seek_rule_queue_seek(ContractTestRunner& runner) {
    runner.run("recovering_seek_rule_queue_seek", []() {
        ASSERT(static_cast<uint8_t>(RecoveringSeekRule::QueueSeek) == 2);
    });
}

void recovering_seek_rule_defer_until_recovered(ContractTestRunner& runner) {
    runner.run("recovering_seek_rule_defer_until_recovered", []() {
        ASSERT(static_cast<uint8_t>(RecoveringSeekRule::DeferUntilRecovered) == 3);
    });
}

void recovering_seek_rule_fail_if_recovery_required(ContractTestRunner& runner) {
    runner.run("recovering_seek_rule_fail_if_recovery_required", []() {
        ASSERT(static_cast<uint8_t>(RecoveringSeekRule::FailIfRecoveryRequired) == 4);
    });
}

void draining_pause_rule_default_is_unknown(ContractTestRunner& runner) {
    runner.run("draining_pause_rule_default_is_unknown", []() {
        DrainingPauseRule r{};
        ASSERT(r == DrainingPauseRule::Unknown);
    });
}

void draining_pause_rule_reject_pause(ContractTestRunner& runner) {
    runner.run("draining_pause_rule_reject_pause", []() {
        ASSERT(static_cast<uint8_t>(DrainingPauseRule::RejectPause) == 1);
    });
}

void draining_pause_rule_queue_pause(ContractTestRunner& runner) {
    runner.run("draining_pause_rule_queue_pause", []() {
        ASSERT(static_cast<uint8_t>(DrainingPauseRule::QueuePause) == 2);
    });
}

void draining_pause_rule_complete_drain_first(ContractTestRunner& runner) {
    runner.run("draining_pause_rule_complete_drain_first", []() {
        ASSERT(static_cast<uint8_t>(DrainingPauseRule::CompleteDrainFirst) == 3);
    });
}

void draining_pause_rule_convert_to_stop(ContractTestRunner& runner) {
    runner.run("draining_pause_rule_convert_to_stop", []() {
        ASSERT(static_cast<uint8_t>(DrainingPauseRule::ConvertToStop) == 4);
    });
}

void failed_close_rule_default_is_unknown(ContractTestRunner& runner) {
    runner.run("failed_close_rule_default_is_unknown", []() {
        FailedCloseRule r{};
        ASSERT(r == FailedCloseRule::Unknown);
    });
}

void failed_close_rule_close_must_succeed(ContractTestRunner& runner) {
    runner.run("failed_close_rule_close_must_succeed", []() {
        ASSERT(static_cast<uint8_t>(FailedCloseRule::CloseMustSucceed) == 1);
    });
}

void failed_close_rule_close_is_idempotent(ContractTestRunner& runner) {
    runner.run("failed_close_rule_close_is_idempotent", []() {
        ASSERT(static_cast<uint8_t>(FailedCloseRule::CloseIsIdempotent) == 2);
    });
}

void failed_close_rule_cleanup_then_close(ContractTestRunner& runner) {
    runner.run("failed_close_rule_cleanup_then_close", []() {
        ASSERT(static_cast<uint8_t>(FailedCloseRule::CleanupThenClose) == 3);
    });
}

void closed_mutation_rule_default_is_unknown(ContractTestRunner& runner) {
    runner.run("closed_mutation_rule_default_is_unknown", []() {
        ClosedMutationRule r{};
        ASSERT(r == ClosedMutationRule::Unknown);
    });
}

void closed_mutation_rule_reject_mutation(ContractTestRunner& runner) {
    runner.run("closed_mutation_rule_reject_mutation", []() {
        ASSERT(static_cast<uint8_t>(ClosedMutationRule::RejectMutation) == 1);
    });
}

void closed_mutation_rule_return_already_closed(ContractTestRunner& runner) {
    runner.run("closed_mutation_rule_return_already_closed", []() {
        ASSERT(static_cast<uint8_t>(ClosedMutationRule::ReturnAlreadyClosed) == 2);
    });
}

void closed_mutation_rule_inspect_only(ContractTestRunner& runner) {
    runner.run("closed_mutation_rule_inspect_only", []() {
        ASSERT(static_cast<uint8_t>(ClosedMutationRule::InspectOnly) == 3);
    });
}

void device_lost_during_seek_rule_default_is_unknown(ContractTestRunner& runner) {
    runner.run("device_lost_during_seek_rule_default_is_unknown", []() {
        DeviceLostDuringSeekRule r{};
        ASSERT(r == DeviceLostDuringSeekRule::Unknown);
    });
}

void device_lost_during_seek_rule_enter_recovering(ContractTestRunner& runner) {
    runner.run("device_lost_during_seek_rule_enter_recovering", []() {
        ASSERT(static_cast<uint8_t>(DeviceLostDuringSeekRule::EnterRecovering) == 1);
    });
}

void device_lost_during_seek_rule_fail_seek(ContractTestRunner& runner) {
    runner.run("device_lost_during_seek_rule_fail_seek", []() {
        ASSERT(static_cast<uint8_t>(DeviceLostDuringSeekRule::FailSeek) == 2);
    });
}

void device_lost_during_seek_rule_preserve_seek_target(ContractTestRunner& runner) {
    runner.run("device_lost_during_seek_rule_preserve_seek_target", []() {
        ASSERT(static_cast<uint8_t>(DeviceLostDuringSeekRule::PreserveSeekTarget) == 3);
    });
}

void device_lost_during_seek_rule_require_recovery_then_seek(ContractTestRunner& runner) {
    runner.run("device_lost_during_seek_rule_require_recovery_then_seek", []() {
        ASSERT(static_cast<uint8_t>(DeviceLostDuringSeekRule::RequireRecoveryThenSeek) == 4);
    });
}

void shutdown_during_drain_rule_default_is_unknown(ContractTestRunner& runner) {
    runner.run("shutdown_during_drain_rule_default_is_unknown", []() {
        ShutdownDuringDrainRule r{};
        ASSERT(r == ShutdownDuringDrainRule::Unknown);
    });
}

void shutdown_during_drain_rule_complete_drain_then_close(ContractTestRunner& runner) {
    runner.run("shutdown_during_drain_rule_complete_drain_then_close", []() {
        ASSERT(static_cast<uint8_t>(ShutdownDuringDrainRule::CompleteDrainThenClose) == 1);
    });
}

void shutdown_during_drain_rule_abort_drain_then_close(ContractTestRunner& runner) {
    runner.run("shutdown_during_drain_rule_abort_drain_then_close", []() {
        ASSERT(static_cast<uint8_t>(ShutdownDuringDrainRule::AbortDrainThenClose) == 2);
    });
}

void shutdown_during_drain_rule_force_close(ContractTestRunner& runner) {
    runner.run("shutdown_during_drain_rule_force_close", []() {
        ASSERT(static_cast<uint8_t>(ShutdownDuringDrainRule::ForceClose) == 3);
    });
}

void shutdown_during_drain_rule_mark_closed_idempotent(ContractTestRunner& runner) {
    runner.run("shutdown_during_drain_rule_mark_closed_idempotent", []() {
        ASSERT(static_cast<uint8_t>(ShutdownDuringDrainRule::MarkClosedIdempotent) == 4);
    });
}

} // namespace

void run_state_scenario_tests(ContractTestRunner& runner) {
    seeking_reentry_rule_default_is_unknown(runner);
    seeking_reentry_rule_merge_seek(runner);
    seeking_reentry_rule_queue_seek(runner);
    seeking_reentry_rule_replace_with_latest_seek(runner);
    seeking_reentry_rule_reject_new_seek(runner);
    recovering_seek_rule_default_is_unknown(runner);
    recovering_seek_rule_reject_seek(runner);
    recovering_seek_rule_queue_seek(runner);
    recovering_seek_rule_defer_until_recovered(runner);
    recovering_seek_rule_fail_if_recovery_required(runner);
    draining_pause_rule_default_is_unknown(runner);
    draining_pause_rule_reject_pause(runner);
    draining_pause_rule_queue_pause(runner);
    draining_pause_rule_complete_drain_first(runner);
    draining_pause_rule_convert_to_stop(runner);
    failed_close_rule_default_is_unknown(runner);
    failed_close_rule_close_must_succeed(runner);
    failed_close_rule_close_is_idempotent(runner);
    failed_close_rule_cleanup_then_close(runner);
    closed_mutation_rule_default_is_unknown(runner);
    closed_mutation_rule_reject_mutation(runner);
    closed_mutation_rule_return_already_closed(runner);
    closed_mutation_rule_inspect_only(runner);
    device_lost_during_seek_rule_default_is_unknown(runner);
    device_lost_during_seek_rule_enter_recovering(runner);
    device_lost_during_seek_rule_fail_seek(runner);
    device_lost_during_seek_rule_preserve_seek_target(runner);
    device_lost_during_seek_rule_require_recovery_then_seek(runner);
    shutdown_during_drain_rule_default_is_unknown(runner);
    shutdown_during_drain_rule_complete_drain_then_close(runner);
    shutdown_during_drain_rule_abort_drain_then_close(runner);
    shutdown_during_drain_rule_force_close(runner);
    shutdown_during_drain_rule_mark_closed_idempotent(runner);
}
