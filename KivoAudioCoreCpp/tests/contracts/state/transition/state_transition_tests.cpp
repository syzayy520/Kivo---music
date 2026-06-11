#include "../../contract_tests_main.h"
#include "kivo/core/contract/state/transition/state_transition_intent.hpp"
#include "kivo/core/contract/state/transition/state_transition.hpp"
#include "kivo/core/contract/state/transition/state_transition_validity.hpp"
#include "kivo/core/contract/state/transition/state_transition_decision.hpp"

using namespace kivo::core::contract;

namespace {

void state_transition_intent_default_is_unknown(ContractTestRunner& runner) {
    runner.run("state_transition_intent_default_is_unknown", []() {
        StateTransitionIntent i{};
        ASSERT(i == StateTransitionIntent::Unknown);
    });
}

void state_transition_intent_open(ContractTestRunner& runner) {
    runner.run("state_transition_intent_open", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionIntent::Open) == 1);
    });
}

void state_transition_intent_prepare(ContractTestRunner& runner) {
    runner.run("state_transition_intent_prepare", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionIntent::Prepare) == 2);
    });
}

void state_transition_intent_start(ContractTestRunner& runner) {
    runner.run("state_transition_intent_start", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionIntent::Start) == 3);
    });
}

void state_transition_intent_pause(ContractTestRunner& runner) {
    runner.run("state_transition_intent_pause", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionIntent::Pause) == 4);
    });
}

void state_transition_intent_resume(ContractTestRunner& runner) {
    runner.run("state_transition_intent_resume", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionIntent::Resume) == 5);
    });
}

void state_transition_intent_seek(ContractTestRunner& runner) {
    runner.run("state_transition_intent_seek", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionIntent::Seek) == 6);
    });
}

void state_transition_intent_drain(ContractTestRunner& runner) {
    runner.run("state_transition_intent_drain", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionIntent::Drain) == 7);
    });
}

void state_transition_intent_recover(ContractTestRunner& runner) {
    runner.run("state_transition_intent_recover", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionIntent::Recover) == 8);
    });
}

void state_transition_intent_stop(ContractTestRunner& runner) {
    runner.run("state_transition_intent_stop", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionIntent::Stop) == 9);
    });
}

void state_transition_intent_close(ContractTestRunner& runner) {
    runner.run("state_transition_intent_close", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionIntent::Close) == 10);
    });
}

void state_transition_intent_fail(ContractTestRunner& runner) {
    runner.run("state_transition_intent_fail", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionIntent::Fail) == 11);
    });
}

void state_transition_intent_reset(ContractTestRunner& runner) {
    runner.run("state_transition_intent_reset", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionIntent::Reset) == 12);
    });
}

void state_transition_default_construction(ContractTestRunner& runner) {
    runner.run("state_transition_default_construction", []() {
        StateTransition t{};
        ASSERT(t.from == CoreState::Unknown);
        ASSERT(t.to == CoreState::Unknown);
        ASSERT(t.intent == StateTransitionIntent::Unknown);
    });
}

void state_transition_field_modification(ContractTestRunner& runner) {
    runner.run("state_transition_field_modification", []() {
        StateTransition t{};
        t.from = CoreState::Idle;
        t.to = CoreState::Playing;
        t.intent = StateTransitionIntent::Start;
        ASSERT(t.from == CoreState::Idle);
        ASSERT(t.to == CoreState::Playing);
        ASSERT(t.intent == StateTransitionIntent::Start);
    });
}

void state_transition_equality(ContractTestRunner& runner) {
    runner.run("state_transition_equality", []() {
        StateTransition a{CoreState::Idle, CoreState::Playing, StateTransitionIntent::Start};
        StateTransition b{CoreState::Idle, CoreState::Playing, StateTransitionIntent::Start};
        StateTransition c{CoreState::Idle, CoreState::Paused, StateTransitionIntent::Pause};
        ASSERT(a == b);
        ASSERT(!(a == c));
    });
}

void state_transition_validity_default_is_unknown(ContractTestRunner& runner) {
    runner.run("state_transition_validity_default_is_unknown", []() {
        StateTransitionValidity v{};
        ASSERT(v == StateTransitionValidity::Unknown);
    });
}

void state_transition_validity_allowed(ContractTestRunner& runner) {
    runner.run("state_transition_validity_allowed", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionValidity::Allowed) == 1);
    });
}

void state_transition_validity_forbidden(ContractTestRunner& runner) {
    runner.run("state_transition_validity_forbidden", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionValidity::Forbidden) == 2);
    });
}

void state_transition_validity_conditional(ContractTestRunner& runner) {
    runner.run("state_transition_validity_conditional", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionValidity::Conditional) == 3);
    });
}

void state_transition_validity_merge_required(ContractTestRunner& runner) {
    runner.run("state_transition_validity_merge_required", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionValidity::MergeRequired) == 4);
    });
}

void state_transition_validity_queue_required(ContractTestRunner& runner) {
    runner.run("state_transition_validity_queue_required", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionValidity::QueueRequired) == 5);
    });
}

void state_transition_validity_reject_required(ContractTestRunner& runner) {
    runner.run("state_transition_validity_reject_required", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionValidity::RejectRequired) == 6);
    });
}

void state_transition_validity_complete_current_first(ContractTestRunner& runner) {
    runner.run("state_transition_validity_complete_current_first", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionValidity::CompleteCurrentFirst) == 7);
    });
}

void state_transition_decision_default_is_unknown(ContractTestRunner& runner) {
    runner.run("state_transition_decision_default_is_unknown", []() {
        StateTransitionDecision d{};
        ASSERT(d == StateTransitionDecision::Unknown);
    });
}

void state_transition_decision_allow(ContractTestRunner& runner) {
    runner.run("state_transition_decision_allow", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionDecision::Allow) == 1);
    });
}

void state_transition_decision_reject(ContractTestRunner& runner) {
    runner.run("state_transition_decision_reject", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionDecision::Reject) == 2);
    });
}

void state_transition_decision_queue(ContractTestRunner& runner) {
    runner.run("state_transition_decision_queue", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionDecision::Queue) == 3);
    });
}

void state_transition_decision_merge(ContractTestRunner& runner) {
    runner.run("state_transition_decision_merge", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionDecision::Merge) == 4);
    });
}

void state_transition_decision_defer(ContractTestRunner& runner) {
    runner.run("state_transition_decision_defer", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionDecision::Defer) == 5);
    });
}

void state_transition_decision_force_close(ContractTestRunner& runner) {
    runner.run("state_transition_decision_force_close", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionDecision::ForceClose) == 6);
    });
}

} // namespace

void run_state_transition_tests(ContractTestRunner& runner) {
    state_transition_intent_default_is_unknown(runner);
    state_transition_intent_open(runner);
    state_transition_intent_prepare(runner);
    state_transition_intent_start(runner);
    state_transition_intent_pause(runner);
    state_transition_intent_resume(runner);
    state_transition_intent_seek(runner);
    state_transition_intent_drain(runner);
    state_transition_intent_recover(runner);
    state_transition_intent_stop(runner);
    state_transition_intent_close(runner);
    state_transition_intent_fail(runner);
    state_transition_intent_reset(runner);
    state_transition_default_construction(runner);
    state_transition_field_modification(runner);
    state_transition_equality(runner);
    state_transition_validity_default_is_unknown(runner);
    state_transition_validity_allowed(runner);
    state_transition_validity_forbidden(runner);
    state_transition_validity_conditional(runner);
    state_transition_validity_merge_required(runner);
    state_transition_validity_queue_required(runner);
    state_transition_validity_reject_required(runner);
    state_transition_validity_complete_current_first(runner);
    state_transition_decision_default_is_unknown(runner);
    state_transition_decision_allow(runner);
    state_transition_decision_reject(runner);
    state_transition_decision_queue(runner);
    state_transition_decision_merge(runner);
    state_transition_decision_defer(runner);
    state_transition_decision_force_close(runner);
}
