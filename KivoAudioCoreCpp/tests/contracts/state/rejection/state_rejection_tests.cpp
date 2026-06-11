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

void state_transition_rejection_reason_invalid_source_state(ContractTestRunner& runner) {
    runner.run("state_transition_rejection_reason_invalid_source_state", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionRejectionReason::InvalidSourceState) == 1);
    });
}

void state_transition_rejection_reason_invalid_target_state(ContractTestRunner& runner) {
    runner.run("state_transition_rejection_reason_invalid_target_state", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionRejectionReason::InvalidTargetState) == 2);
    });
}

void state_transition_rejection_reason_terminal_state(ContractTestRunner& runner) {
    runner.run("state_transition_rejection_reason_terminal_state", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionRejectionReason::TerminalState) == 3);
    });
}

void state_transition_rejection_reason_already_in_requested_state(ContractTestRunner& runner) {
    runner.run("state_transition_rejection_reason_already_in_requested_state", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionRejectionReason::AlreadyInRequestedState) == 4);
    });
}

void state_transition_rejection_reason_conflicting_transition(ContractTestRunner& runner) {
    runner.run("state_transition_rejection_reason_conflicting_transition", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionRejectionReason::ConflictingTransition) == 5);
    });
}

void state_transition_rejection_reason_requires_drain_completion(ContractTestRunner& runner) {
    runner.run("state_transition_rejection_reason_requires_drain_completion", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionRejectionReason::RequiresDrainCompletion) == 6);
    });
}

void state_transition_rejection_reason_requires_recovery_completion(ContractTestRunner& runner) {
    runner.run("state_transition_rejection_reason_requires_recovery_completion", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionRejectionReason::RequiresRecoveryCompletion) == 7);
    });
}

void state_transition_rejection_reason_closed_state_rejects_mutation(ContractTestRunner& runner) {
    runner.run("state_transition_rejection_reason_closed_state_rejects_mutation", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionRejectionReason::ClosedStateRejectsMutation) == 8);
    });
}

void state_transition_rejection_reason_failed_state_allows_close_only(ContractTestRunner& runner) {
    runner.run("state_transition_rejection_reason_failed_state_allows_close_only", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionRejectionReason::FailedStateAllowsCloseOnly) == 9);
    });
}

void illegal_transition_rule_default_construction(ContractTestRunner& runner) {
    runner.run("illegal_transition_rule_default_construction", []() {
        IllegalTransitionRule rule{};
        ASSERT(rule.transition.from == CoreState::Unknown);
        ASSERT(rule.transition.to == CoreState::Unknown);
        ASSERT(rule.transition.intent == StateTransitionIntent::Unknown);
        ASSERT(rule.validity == StateTransitionValidity::Unknown);
        ASSERT(rule.rejection_reason == StateTransitionRejectionReason::Unknown);
    });
}

void illegal_transition_rule_field_modification(ContractTestRunner& runner) {
    runner.run("illegal_transition_rule_field_modification", []() {
        IllegalTransitionRule rule{};
        rule.transition.from = CoreState::Playing;
        rule.transition.to = CoreState::Idle;
        rule.transition.intent = StateTransitionIntent::Stop;
        rule.validity = StateTransitionValidity::Forbidden;
        rule.rejection_reason = StateTransitionRejectionReason::InvalidSourceState;
        ASSERT(rule.transition.from == CoreState::Playing);
        ASSERT(rule.transition.to == CoreState::Idle);
        ASSERT(rule.transition.intent == StateTransitionIntent::Stop);
        ASSERT(rule.validity == StateTransitionValidity::Forbidden);
        ASSERT(rule.rejection_reason == StateTransitionRejectionReason::InvalidSourceState);
    });
}

void illegal_transition_rule_equality(ContractTestRunner& runner) {
    runner.run("illegal_transition_rule_equality", []() {
        IllegalTransitionRule a{};
        a.transition = {CoreState::Playing, CoreState::Idle, StateTransitionIntent::Stop};
        a.validity = StateTransitionValidity::Forbidden;
        a.rejection_reason = StateTransitionRejectionReason::InvalidSourceState;

        IllegalTransitionRule b{};
        b.transition = {CoreState::Playing, CoreState::Idle, StateTransitionIntent::Stop};
        b.validity = StateTransitionValidity::Forbidden;
        b.rejection_reason = StateTransitionRejectionReason::InvalidSourceState;

        IllegalTransitionRule c{};
        c.transition = {CoreState::Playing, CoreState::Idle, StateTransitionIntent::Stop};
        c.validity = StateTransitionValidity::Forbidden;
        c.rejection_reason = StateTransitionRejectionReason::TerminalState;

        ASSERT(a == b);
        ASSERT(!(a == c));
    });
}

} // namespace

void run_state_rejection_tests(ContractTestRunner& runner) {
    state_transition_rejection_reason_default_is_unknown(runner);
    state_transition_rejection_reason_invalid_source_state(runner);
    state_transition_rejection_reason_invalid_target_state(runner);
    state_transition_rejection_reason_terminal_state(runner);
    state_transition_rejection_reason_already_in_requested_state(runner);
    state_transition_rejection_reason_conflicting_transition(runner);
    state_transition_rejection_reason_requires_drain_completion(runner);
    state_transition_rejection_reason_requires_recovery_completion(runner);
    state_transition_rejection_reason_closed_state_rejects_mutation(runner);
    state_transition_rejection_reason_failed_state_allows_close_only(runner);
    illegal_transition_rule_default_construction(runner);
    illegal_transition_rule_field_modification(runner);
    illegal_transition_rule_equality(runner);
}
