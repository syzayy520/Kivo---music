#include "../../contract_tests_main.h"
#include "kivo/core/contract/state/rejection/state_transition_rejection_reason.hpp"
#include "kivo/core/contract/state/rejection/illegal_transition_rule.hpp"

using namespace kivo::core::contract;

namespace {

void state_transition_rejection_reason_default_is_unknown(ContractTestRunner& runner) {
    runner.run("state_transition_rejection_reason_default_is_unknown", []() {
        StateTransitionRejectionReason r{};
        ASSERT(r == StateTransitionRejectionReason::Unknown);
    });
}

void state_transition_rejection_reason_illegal_transition(ContractTestRunner& runner) {
    runner.run("state_transition_rejection_reason_illegal_transition", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionRejectionReason::IllegalTransition) == 1);
    });
}

void state_transition_rejection_reason_terminal_state(ContractTestRunner& runner) {
    runner.run("state_transition_rejection_reason_terminal_state", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionRejectionReason::TerminalState) == 2);
    });
}

void state_transition_rejection_reason_concurrent_transition(ContractTestRunner& runner) {
    runner.run("state_transition_rejection_reason_concurrent_transition", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionRejectionReason::ConcurrentTransition) == 3);
    });
}

void state_transition_rejection_reason_generation_mismatch(ContractTestRunner& runner) {
    runner.run("state_transition_rejection_reason_generation_mismatch", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionRejectionReason::GenerationMismatch) == 4);
    });
}

void state_transition_rejection_reason_rule_violation(ContractTestRunner& runner) {
    runner.run("state_transition_rejection_reason_rule_violation", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionRejectionReason::RuleViolation) == 5);
    });
}

void state_transition_rejection_reason_scenario_conflict(ContractTestRunner& runner) {
    runner.run("state_transition_rejection_reason_scenario_conflict", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionRejectionReason::ScenarioConflict) == 6);
    });
}

void illegal_transition_rule_default_construction(ContractTestRunner& runner) {
    runner.run("illegal_transition_rule_default_construction", []() {
        IllegalTransitionRule rule{};
        ASSERT(rule.from_state == CoreState::Unknown);
        ASSERT(rule.to_state == CoreState::Unknown);
        ASSERT(rule.decision == StateTransitionDecision::Reject);
    });
}

void illegal_transition_rule_field_modification(ContractTestRunner& runner) {
    runner.run("illegal_transition_rule_field_modification", []() {
        IllegalTransitionRule rule{};
        rule.from_state = CoreState::Playing;
        rule.to_state = CoreState::Idle;
        rule.decision = StateTransitionDecision::Reject;
        ASSERT(rule.from_state == CoreState::Playing);
        ASSERT(rule.to_state == CoreState::Idle);
        ASSERT(rule.decision == StateTransitionDecision::Reject);
    });
}

void illegal_transition_rule_equality(ContractTestRunner& runner) {
    runner.run("illegal_transition_rule_equality", []() {
        IllegalTransitionRule a{CoreState::Playing, CoreState::Idle, StateTransitionDecision::Reject};
        IllegalTransitionRule b{CoreState::Playing, CoreState::Idle, StateTransitionDecision::Reject};
        IllegalTransitionRule c{CoreState::Playing, CoreState::Idle, StateTransitionDecision::Queue};
        ASSERT(a == b);
        ASSERT(!(a == c));
    });
}

} // namespace

void run_state_rejection_tests(ContractTestRunner& runner) {
    state_transition_rejection_reason_default_is_unknown(runner);
    state_transition_rejection_reason_illegal_transition(runner);
    state_transition_rejection_reason_terminal_state(runner);
    state_transition_rejection_reason_concurrent_transition(runner);
    state_transition_rejection_reason_generation_mismatch(runner);
    state_transition_rejection_reason_rule_violation(runner);
    state_transition_rejection_reason_scenario_conflict(runner);
    illegal_transition_rule_default_construction(runner);
    illegal_transition_rule_field_modification(runner);
    illegal_transition_rule_equality(runner);
}