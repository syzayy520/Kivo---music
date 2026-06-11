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

void reentrant_transition_policy_forbid(ContractTestRunner& runner) {
    runner.run("reentrant_transition_policy_forbid", []() {
        ASSERT(static_cast<uint8_t>(ReentrantTransitionPolicy::Forbid) == 1);
    });
}

void reentrant_transition_policy_merge(ContractTestRunner& runner) {
    runner.run("reentrant_transition_policy_merge", []() {
        ASSERT(static_cast<uint8_t>(ReentrantTransitionPolicy::Merge) == 2);
    });
}

void reentrant_transition_policy_queue(ContractTestRunner& runner) {
    runner.run("reentrant_transition_policy_queue", []() {
        ASSERT(static_cast<uint8_t>(ReentrantTransitionPolicy::Queue) == 3);
    });
}

void reentrant_transition_policy_replace_pending(ContractTestRunner& runner) {
    runner.run("reentrant_transition_policy_replace_pending", []() {
        ASSERT(static_cast<uint8_t>(ReentrantTransitionPolicy::ReplacePending) == 4);
    });
}

void reentrant_transition_policy_keep_latest(ContractTestRunner& runner) {
    runner.run("reentrant_transition_policy_keep_latest", []() {
        ASSERT(static_cast<uint8_t>(ReentrantTransitionPolicy::KeepLatest) == 5);
    });
}

void transition_preemption_policy_default_is_unknown(ContractTestRunner& runner) {
    runner.run("transition_preemption_policy_default_is_unknown", []() {
        TransitionPreemptionPolicy p{};
        ASSERT(p == TransitionPreemptionPolicy::Unknown);
    });
}

void transition_preemption_policy_never_preempt(ContractTestRunner& runner) {
    runner.run("transition_preemption_policy_never_preempt", []() {
        ASSERT(static_cast<uint8_t>(TransitionPreemptionPolicy::NeverPreempt) == 1);
    });
}

void transition_preemption_policy_allow_safe_preempt(ContractTestRunner& runner) {
    runner.run("transition_preemption_policy_allow_safe_preempt", []() {
        ASSERT(static_cast<uint8_t>(TransitionPreemptionPolicy::AllowSafePreempt) == 2);
    });
}

void transition_preemption_policy_queue_until_current_completes(ContractTestRunner& runner) {
    runner.run("transition_preemption_policy_queue_until_current_completes", []() {
        ASSERT(static_cast<uint8_t>(TransitionPreemptionPolicy::QueueUntilCurrentCompletes) == 3);
    });
}

void transition_preemption_policy_reject_until_current_completes(ContractTestRunner& runner) {
    runner.run("transition_preemption_policy_reject_until_current_completes", []() {
        ASSERT(static_cast<uint8_t>(TransitionPreemptionPolicy::RejectUntilCurrentCompletes) == 4);
    });
}

void transition_preemption_policy_force_close_only(ContractTestRunner& runner) {
    runner.run("transition_preemption_policy_force_close_only", []() {
        ASSERT(static_cast<uint8_t>(TransitionPreemptionPolicy::ForceCloseOnly) == 5);
    });
}

void terminal_state_rule_default_is_unknown(ContractTestRunner& runner) {
    runner.run("terminal_state_rule_default_is_unknown", []() {
        TerminalStateRule r{};
        ASSERT(r == TerminalStateRule::Unknown);
    });
}

void terminal_state_rule_reject_all_mutations(ContractTestRunner& runner) {
    runner.run("terminal_state_rule_reject_all_mutations", []() {
        ASSERT(static_cast<uint8_t>(TerminalStateRule::RejectAllMutations) == 1);
    });
}

void terminal_state_rule_allow_close_only(ContractTestRunner& runner) {
    runner.run("terminal_state_rule_allow_close_only", []() {
        ASSERT(static_cast<uint8_t>(TerminalStateRule::AllowCloseOnly) == 2);
    });
}

void terminal_state_rule_allow_inspect_only(ContractTestRunner& runner) {
    runner.run("terminal_state_rule_allow_inspect_only", []() {
        ASSERT(static_cast<uint8_t>(TerminalStateRule::AllowInspectOnly) == 3);
    });
}

void terminal_state_rule_already_closed_is_idempotent(ContractTestRunner& runner) {
    runner.run("terminal_state_rule_already_closed_is_idempotent", []() {
        ASSERT(static_cast<uint8_t>(TerminalStateRule::AlreadyClosedIsIdempotent) == 4);
    });
}

} // namespace

void run_state_rule_tests(ContractTestRunner& runner) {
    reentrant_transition_policy_default_is_unknown(runner);
    reentrant_transition_policy_forbid(runner);
    reentrant_transition_policy_merge(runner);
    reentrant_transition_policy_queue(runner);
    reentrant_transition_policy_replace_pending(runner);
    reentrant_transition_policy_keep_latest(runner);
    transition_preemption_policy_default_is_unknown(runner);
    transition_preemption_policy_never_preempt(runner);
    transition_preemption_policy_allow_safe_preempt(runner);
    transition_preemption_policy_queue_until_current_completes(runner);
    transition_preemption_policy_reject_until_current_completes(runner);
    transition_preemption_policy_force_close_only(runner);
    terminal_state_rule_default_is_unknown(runner);
    terminal_state_rule_reject_all_mutations(runner);
    terminal_state_rule_allow_close_only(runner);
    terminal_state_rule_allow_inspect_only(runner);
    terminal_state_rule_already_closed_is_idempotent(runner);
}
