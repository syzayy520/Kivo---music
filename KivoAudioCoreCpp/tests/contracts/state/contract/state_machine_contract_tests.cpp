#include "../../contract_tests_main.h"
#include "kivo/core/contract/state/contract/state_machine_contract.hpp"

using namespace kivo::core::contract;

namespace {

void state_machine_contract_default_construction(ContractTestRunner& runner) {
    runner.run("state_machine_contract_default_construction", []() {
        StateMachineContract contract{};
        ASSERT(contract.reentrant_policy == ReentrantTransitionPolicy::Unknown);
        ASSERT(contract.preemption_policy == TransitionPreemptionPolicy::Unknown);
        ASSERT(contract.terminal_rule.terminal_state == CoreState::Unknown);
        ASSERT(contract.terminal_rule.decision == StateTransitionDecision::Reject);
        ASSERT(contract.seeking_reentry_rule.active_state == CoreState::Seeking);
        ASSERT(contract.seeking_reentry_rule.policy == ReentrantTransitionPolicy::Merge);
        ASSERT(contract.recovering_seek_rule.recovering_state == CoreState::Recovering);
        ASSERT(contract.recovering_seek_rule.seek_target == CoreState::Seeking);
        ASSERT(contract.recovering_seek_rule.decision == StateTransitionDecision::Reject);
        ASSERT(contract.draining_pause_rule.draining_state == CoreState::Draining);
        ASSERT(contract.draining_pause_rule.pause_target == CoreState::Paused);
        ASSERT(contract.draining_pause_rule.decision == StateTransitionDecision::Defer);
        ASSERT(contract.failed_close_rule.failed_state == CoreState::Failed);
        ASSERT(contract.failed_close_rule.close_target == CoreState::Closed);
        ASSERT(contract.failed_close_rule.decision == StateTransitionDecision::Allow);
        ASSERT(contract.closed_mutation_rule.closed_state == CoreState::Closed);
        ASSERT(contract.closed_mutation_rule.decision == StateTransitionDecision::Reject);
        ASSERT(contract.device_lost_during_seek_rule.seeking_state == CoreState::Seeking);
        ASSERT(contract.device_lost_during_seek_rule.failed_target == CoreState::Failed);
        ASSERT(contract.device_lost_during_seek_rule.decision == StateTransitionDecision::Allow);
        ASSERT(contract.shutdown_during_drain_rule.draining_state == CoreState::Draining);
        ASSERT(contract.shutdown_during_drain_rule.closed_target == CoreState::Closed);
        ASSERT(contract.shutdown_during_drain_rule.decision == StateTransitionDecision::Allow);
    });
}

void state_machine_contract_field_modification(ContractTestRunner& runner) {
    runner.run("state_machine_contract_field_modification", []() {
        StateMachineContract contract{};
        contract.reentrant_policy = ReentrantTransitionPolicy::Merge;
        contract.preemption_policy = TransitionPreemptionPolicy::AllowPreemption;
        contract.terminal_rule.terminal_state = CoreState::Closed;
        contract.terminal_rule.decision = StateTransitionDecision::Reject;
        contract.seeking_reentry_rule.policy = ReentrantTransitionPolicy::Queue;
        contract.recovering_seek_rule.decision = StateTransitionDecision::Queue;
        contract.draining_pause_rule.decision = StateTransitionDecision::Reject;
        contract.failed_close_rule.decision = StateTransitionDecision::Allow;
        contract.closed_mutation_rule.decision = StateTransitionDecision::Reject;
        contract.device_lost_during_seek_rule.decision = StateTransitionDecision::Allow;
        contract.shutdown_during_drain_rule.decision = StateTransitionDecision::Allow;

        ASSERT(contract.reentrant_policy == ReentrantTransitionPolicy::Merge);
        ASSERT(contract.preemption_policy == TransitionPreemptionPolicy::AllowPreemption);
        ASSERT(contract.terminal_rule.terminal_state == CoreState::Closed);
        ASSERT(contract.terminal_rule.decision == StateTransitionDecision::Reject);
        ASSERT(contract.seeking_reentry_rule.policy == ReentrantTransitionPolicy::Queue);
        ASSERT(contract.recovering_seek_rule.decision == StateTransitionDecision::Queue);
        ASSERT(contract.draining_pause_rule.decision == StateTransitionDecision::Reject);
        ASSERT(contract.failed_close_rule.decision == StateTransitionDecision::Allow);
        ASSERT(contract.closed_mutation_rule.decision == StateTransitionDecision::Reject);
        ASSERT(contract.device_lost_during_seek_rule.decision == StateTransitionDecision::Allow);
        ASSERT(contract.shutdown_during_drain_rule.decision == StateTransitionDecision::Allow);
    });
}

void state_machine_contract_equality(ContractTestRunner& runner) {
    runner.run("state_machine_contract_equality", []() {
        StateMachineContract a{};
        StateMachineContract b{};
        ASSERT(a == b);

        StateMachineContract c{};
        c.reentrant_policy = ReentrantTransitionPolicy::Merge;
        ASSERT(!(a == c));
    });
}

void state_machine_contract_has_all_required_fields(ContractTestRunner& runner) {
    runner.run("state_machine_contract_has_all_required_fields", []() {
        StateMachineContract contract{};
        // Rule fields (3)
        (void)contract.reentrant_policy;
        (void)contract.preemption_policy;
        (void)contract.terminal_rule;
        // Scenario fields (7)
        (void)contract.seeking_reentry_rule;
        (void)contract.recovering_seek_rule;
        (void)contract.draining_pause_rule;
        (void)contract.failed_close_rule;
        (void)contract.closed_mutation_rule;
        (void)contract.device_lost_during_seek_rule;
        (void)contract.shutdown_during_drain_rule;
        ASSERT(true); // Compilation success means all fields exist
    });
}

} // namespace

void run_state_machine_contract_tests(ContractTestRunner& runner) {
    state_machine_contract_default_construction(runner);
    state_machine_contract_field_modification(runner);
    state_machine_contract_equality(runner);
    state_machine_contract_has_all_required_fields(runner);
}