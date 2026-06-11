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

void seeking_reentry_rule_default_construction(ContractTestRunner& runner) {
    runner.run("seeking_reentry_rule_default_construction", []() {
        SeekingReentryRule rule{};
        ASSERT(rule.active_state == CoreState::Seeking);
        ASSERT(rule.policy == ReentrantTransitionPolicy::Merge);
    });
}

void seeking_reentry_rule_field_modification(ContractTestRunner& runner) {
    runner.run("seeking_reentry_rule_field_modification", []() {
        SeekingReentryRule rule{};
        rule.active_state = CoreState::Seeking;
        rule.policy = ReentrantTransitionPolicy::Queue;
        ASSERT(rule.active_state == CoreState::Seeking);
        ASSERT(rule.policy == ReentrantTransitionPolicy::Queue);
    });
}

void seeking_reentry_rule_equality(ContractTestRunner& runner) {
    runner.run("seeking_reentry_rule_equality", []() {
        SeekingReentryRule a{CoreState::Seeking, ReentrantTransitionPolicy::Merge};
        SeekingReentryRule b{CoreState::Seeking, ReentrantTransitionPolicy::Merge};
        SeekingReentryRule c{CoreState::Seeking, ReentrantTransitionPolicy::Queue};
        ASSERT(a == b);
        ASSERT(!(a == c));
    });
}

void recovering_seek_rule_default_construction(ContractTestRunner& runner) {
    runner.run("recovering_seek_rule_default_construction", []() {
        RecoveringSeekRule rule{};
        ASSERT(rule.recovering_state == CoreState::Recovering);
        ASSERT(rule.seek_target == CoreState::Seeking);
        ASSERT(rule.decision == StateTransitionDecision::Reject);
    });
}

void recovering_seek_rule_field_modification(ContractTestRunner& runner) {
    runner.run("recovering_seek_rule_field_modification", []() {
        RecoveringSeekRule rule{};
        rule.recovering_state = CoreState::Recovering;
        rule.seek_target = CoreState::Seeking;
        rule.decision = StateTransitionDecision::Queue;
        ASSERT(rule.recovering_state == CoreState::Recovering);
        ASSERT(rule.seek_target == CoreState::Seeking);
        ASSERT(rule.decision == StateTransitionDecision::Queue);
    });
}

void recovering_seek_rule_equality(ContractTestRunner& runner) {
    runner.run("recovering_seek_rule_equality", []() {
        RecoveringSeekRule a{CoreState::Recovering, CoreState::Seeking, StateTransitionDecision::Reject};
        RecoveringSeekRule b{CoreState::Recovering, CoreState::Seeking, StateTransitionDecision::Reject};
        RecoveringSeekRule c{CoreState::Recovering, CoreState::Seeking, StateTransitionDecision::Queue};
        ASSERT(a == b);
        ASSERT(!(a == c));
    });
}

void draining_pause_rule_default_construction(ContractTestRunner& runner) {
    runner.run("draining_pause_rule_default_construction", []() {
        DrainingPauseRule rule{};
        ASSERT(rule.draining_state == CoreState::Draining);
        ASSERT(rule.pause_target == CoreState::Paused);
        ASSERT(rule.decision == StateTransitionDecision::Defer);
    });
}

void draining_pause_rule_field_modification(ContractTestRunner& runner) {
    runner.run("draining_pause_rule_field_modification", []() {
        DrainingPauseRule rule{};
        rule.draining_state = CoreState::Draining;
        rule.pause_target = CoreState::Paused;
        rule.decision = StateTransitionDecision::Reject;
        ASSERT(rule.draining_state == CoreState::Draining);
        ASSERT(rule.pause_target == CoreState::Paused);
        ASSERT(rule.decision == StateTransitionDecision::Reject);
    });
}

void draining_pause_rule_equality(ContractTestRunner& runner) {
    runner.run("draining_pause_rule_equality", []() {
        DrainingPauseRule a{CoreState::Draining, CoreState::Paused, StateTransitionDecision::Defer};
        DrainingPauseRule b{CoreState::Draining, CoreState::Paused, StateTransitionDecision::Defer};
        DrainingPauseRule c{CoreState::Draining, CoreState::Paused, StateTransitionDecision::Reject};
        ASSERT(a == b);
        ASSERT(!(a == c));
    });
}

void failed_close_rule_default_construction(ContractTestRunner& runner) {
    runner.run("failed_close_rule_default_construction", []() {
        FailedCloseRule rule{};
        ASSERT(rule.failed_state == CoreState::Failed);
        ASSERT(rule.close_target == CoreState::Closed);
        ASSERT(rule.decision == StateTransitionDecision::Allow);
    });
}

void failed_close_rule_field_modification(ContractTestRunner& runner) {
    runner.run("failed_close_rule_field_modification", []() {
        FailedCloseRule rule{};
        rule.failed_state = CoreState::Failed;
        rule.close_target = CoreState::Closed;
        rule.decision = StateTransitionDecision::Allow;
        ASSERT(rule.failed_state == CoreState::Failed);
        ASSERT(rule.close_target == CoreState::Closed);
        ASSERT(rule.decision == StateTransitionDecision::Allow);
    });
}

void failed_close_rule_equality(ContractTestRunner& runner) {
    runner.run("failed_close_rule_equality", []() {
        FailedCloseRule a{CoreState::Failed, CoreState::Closed, StateTransitionDecision::Allow};
        FailedCloseRule b{CoreState::Failed, CoreState::Closed, StateTransitionDecision::Allow};
        FailedCloseRule c{CoreState::Failed, CoreState::Closed, StateTransitionDecision::Reject};
        ASSERT(a == b);
        ASSERT(!(a == c));
    });
}

void closed_mutation_rule_default_construction(ContractTestRunner& runner) {
    runner.run("closed_mutation_rule_default_construction", []() {
        ClosedMutationRule rule{};
        ASSERT(rule.closed_state == CoreState::Closed);
        ASSERT(rule.decision == StateTransitionDecision::Reject);
    });
}

void closed_mutation_rule_field_modification(ContractTestRunner& runner) {
    runner.run("closed_mutation_rule_field_modification", []() {
        ClosedMutationRule rule{};
        rule.closed_state = CoreState::Closed;
        rule.decision = StateTransitionDecision::Reject;
        ASSERT(rule.closed_state == CoreState::Closed);
        ASSERT(rule.decision == StateTransitionDecision::Reject);
    });
}

void closed_mutation_rule_equality(ContractTestRunner& runner) {
    runner.run("closed_mutation_rule_equality", []() {
        ClosedMutationRule a{CoreState::Closed, StateTransitionDecision::Reject};
        ClosedMutationRule b{CoreState::Closed, StateTransitionDecision::Reject};
        ClosedMutationRule c{CoreState::Closed, StateTransitionDecision::Allow};
        ASSERT(a == b);
        ASSERT(!(a == c));
    });
}

void device_lost_during_seek_rule_default_construction(ContractTestRunner& runner) {
    runner.run("device_lost_during_seek_rule_default_construction", []() {
        DeviceLostDuringSeekRule rule{};
        ASSERT(rule.seeking_state == CoreState::Seeking);
        ASSERT(rule.failed_target == CoreState::Failed);
        ASSERT(rule.decision == StateTransitionDecision::Allow);
    });
}

void device_lost_during_seek_rule_field_modification(ContractTestRunner& runner) {
    runner.run("device_lost_during_seek_rule_field_modification", []() {
        DeviceLostDuringSeekRule rule{};
        rule.seeking_state = CoreState::Seeking;
        rule.failed_target = CoreState::Failed;
        rule.decision = StateTransitionDecision::Allow;
        ASSERT(rule.seeking_state == CoreState::Seeking);
        ASSERT(rule.failed_target == CoreState::Failed);
        ASSERT(rule.decision == StateTransitionDecision::Allow);
    });
}

void device_lost_during_seek_rule_equality(ContractTestRunner& runner) {
    runner.run("device_lost_during_seek_rule_equality", []() {
        DeviceLostDuringSeekRule a{CoreState::Seeking, CoreState::Failed, StateTransitionDecision::Allow};
        DeviceLostDuringSeekRule b{CoreState::Seeking, CoreState::Failed, StateTransitionDecision::Allow};
        DeviceLostDuringSeekRule c{CoreState::Seeking, CoreState::Failed, StateTransitionDecision::Reject};
        ASSERT(a == b);
        ASSERT(!(a == c));
    });
}

void shutdown_during_drain_rule_default_construction(ContractTestRunner& runner) {
    runner.run("shutdown_during_drain_rule_default_construction", []() {
        ShutdownDuringDrainRule rule{};
        ASSERT(rule.draining_state == CoreState::Draining);
        ASSERT(rule.closed_target == CoreState::Closed);
        ASSERT(rule.decision == StateTransitionDecision::Allow);
    });
}

void shutdown_during_drain_rule_field_modification(ContractTestRunner& runner) {
    runner.run("shutdown_during_drain_rule_field_modification", []() {
        ShutdownDuringDrainRule rule{};
        rule.draining_state = CoreState::Draining;
        rule.closed_target = CoreState::Closed;
        rule.decision = StateTransitionDecision::Allow;
        ASSERT(rule.draining_state == CoreState::Draining);
        ASSERT(rule.closed_target == CoreState::Closed);
        ASSERT(rule.decision == StateTransitionDecision::Allow);
    });
}

void shutdown_during_drain_rule_equality(ContractTestRunner& runner) {
    runner.run("shutdown_during_drain_rule_equality", []() {
        ShutdownDuringDrainRule a{CoreState::Draining, CoreState::Closed, StateTransitionDecision::Allow};
        ShutdownDuringDrainRule b{CoreState::Draining, CoreState::Closed, StateTransitionDecision::Allow};
        ShutdownDuringDrainRule c{CoreState::Draining, CoreState::Closed, StateTransitionDecision::Reject};
        ASSERT(a == b);
        ASSERT(!(a == c));
    });
}

} // namespace

void run_state_scenario_tests(ContractTestRunner& runner) {
    seeking_reentry_rule_default_construction(runner);
    seeking_reentry_rule_field_modification(runner);
    seeking_reentry_rule_equality(runner);
    recovering_seek_rule_default_construction(runner);
    recovering_seek_rule_field_modification(runner);
    recovering_seek_rule_equality(runner);
    draining_pause_rule_default_construction(runner);
    draining_pause_rule_field_modification(runner);
    draining_pause_rule_equality(runner);
    failed_close_rule_default_construction(runner);
    failed_close_rule_field_modification(runner);
    failed_close_rule_equality(runner);
    closed_mutation_rule_default_construction(runner);
    closed_mutation_rule_field_modification(runner);
    closed_mutation_rule_equality(runner);
    device_lost_during_seek_rule_default_construction(runner);
    device_lost_during_seek_rule_field_modification(runner);
    device_lost_during_seek_rule_equality(runner);
    shutdown_during_drain_rule_default_construction(runner);
    shutdown_during_drain_rule_field_modification(runner);
    shutdown_during_drain_rule_equality(runner);
}