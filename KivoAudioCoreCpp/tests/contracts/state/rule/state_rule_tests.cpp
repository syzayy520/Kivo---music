#include "../../contract_tests_main.h"
#include "kivo/core/contract/state/rule/reentrant_transition_policy.hpp"
#include "kivo/core/contract/state/rule/transition_preemption_policy.hpp"
#include "kivo/core/contract/state/rule/terminal_state_rule.hpp"

using namespace kivo::core::contract;

namespace {

void reentrant_transition_policy_default_is_unknown(ContractTestRunner& runner) {
    runner.run("reentrant_transition_policy_default_is_unknown", []() {
        ReentrantTransitionPolicy p{};
        ASSERT(p == ReentrantTransitionPolicy::Unknown);
    });
}

void reentrant_transition_policy_merge(ContractTestRunner& runner) {
    runner.run("reentrant_transition_policy_merge", []() {
        ASSERT(static_cast<uint8_t>(ReentrantTransitionPolicy::Merge) == 1);
    });
}

void reentrant_transition_policy_queue(ContractTestRunner& runner) {
    runner.run("reentrant_transition_policy_queue", []() {
        ASSERT(static_cast<uint8_t>(ReentrantTransitionPolicy::Queue) == 2);
    });
}

void reentrant_transition_policy_reject(ContractTestRunner& runner) {
    runner.run("reentrant_transition_policy_reject", []() {
        ASSERT(static_cast<uint8_t>(ReentrantTransitionPolicy::Reject) == 3);
    });
}

void reentrant_transition_policy_cancel(ContractTestRunner& runner) {
    runner.run("reentrant_transition_policy_cancel", []() {
        ASSERT(static_cast<uint8_t>(ReentrantTransitionPolicy::Cancel) == 4);
    });
}

void transition_preemption_policy_default_is_unknown(ContractTestRunner& runner) {
    runner.run("transition_preemption_policy_default_is_unknown", []() {
        TransitionPreemptionPolicy p{};
        ASSERT(p == TransitionPreemptionPolicy::Unknown);
    });
}

void transition_preemption_policy_allow_preemption(ContractTestRunner& runner) {
    runner.run("transition_preemption_policy_allow_preemption", []() {
        ASSERT(static_cast<uint8_t>(TransitionPreemptionPolicy::AllowPreemption) == 1);
    });
}

void transition_preemption_policy_reject_preemption(ContractTestRunner& runner) {
    runner.run("transition_preemption_policy_reject_preemption", []() {
        ASSERT(static_cast<uint8_t>(TransitionPreemptionPolicy::RejectPreemption) == 2);
    });
}

void transition_preemption_policy_queue_behind(ContractTestRunner& runner) {
    runner.run("transition_preemption_policy_queue_behind", []() {
        ASSERT(static_cast<uint8_t>(TransitionPreemptionPolicy::QueueBehind) == 3);
    });
}

void transition_preemption_policy_cancel_current(ContractTestRunner& runner) {
    runner.run("transition_preemption_policy_cancel_current", []() {
        ASSERT(static_cast<uint8_t>(TransitionPreemptionPolicy::CancelCurrent) == 4);
    });
}

void terminal_state_rule_default_construction(ContractTestRunner& runner) {
    runner.run("terminal_state_rule_default_construction", []() {
        TerminalStateRule rule{};
        ASSERT(rule.terminal_state == CoreState::Unknown);
        ASSERT(rule.decision == StateTransitionDecision::Reject);
    });
}

void terminal_state_rule_field_modification(ContractTestRunner& runner) {
    runner.run("terminal_state_rule_field_modification", []() {
        TerminalStateRule rule{};
        rule.terminal_state = CoreState::Closed;
        rule.decision = StateTransitionDecision::Reject;
        ASSERT(rule.terminal_state == CoreState::Closed);
        ASSERT(rule.decision == StateTransitionDecision::Reject);
    });
}

void terminal_state_rule_equality(ContractTestRunner& runner) {
    runner.run("terminal_state_rule_equality", []() {
        TerminalStateRule a{CoreState::Closed, StateTransitionDecision::Reject};
        TerminalStateRule b{CoreState::Closed, StateTransitionDecision::Reject};
        TerminalStateRule c{CoreState::Closed, StateTransitionDecision::Allow};
        ASSERT(a == b);
        ASSERT(!(a == c));
    });
}

} // namespace

void run_state_rule_tests(ContractTestRunner& runner) {
    reentrant_transition_policy_default_is_unknown(runner);
    reentrant_transition_policy_merge(runner);
    reentrant_transition_policy_queue(runner);
    reentrant_transition_policy_reject(runner);
    reentrant_transition_policy_cancel(runner);
    transition_preemption_policy_default_is_unknown(runner);
    transition_preemption_policy_allow_preemption(runner);
    transition_preemption_policy_reject_preemption(runner);
    transition_preemption_policy_queue_behind(runner);
    transition_preemption_policy_cancel_current(runner);
    terminal_state_rule_default_construction(runner);
    terminal_state_rule_field_modification(runner);
    terminal_state_rule_equality(runner);
}