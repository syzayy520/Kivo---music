#include "../../contract_tests_main.h"
#include "kivo/core/contract/state/transition/state_transition_intent.hpp"
#include "kivo/core/contract/state/transition/state_transition.hpp"
#include "kivo/core/contract/state/transition/state_transition_validity.hpp"
#include "kivo/core/contract/state/transition/state_transition_decision.hpp"

using namespace kivo::core::contract;

namespace {

void state_transition_intent_default_construction(ContractTestRunner& runner) {
    runner.run("state_transition_intent_default_construction", []() {
        StateTransitionIntent intent{};
        ASSERT(intent.from_state == CoreState::Unknown);
        ASSERT(intent.to_state == CoreState::Unknown);
        ASSERT(intent.trigger_id == 0);
    });
}

void state_transition_intent_field_modification(ContractTestRunner& runner) {
    runner.run("state_transition_intent_field_modification", []() {
        StateTransitionIntent intent{};
        intent.from_state = CoreState::Idle;
        intent.to_state = CoreState::Playing;
        intent.trigger_id = 42;
        ASSERT(intent.from_state == CoreState::Idle);
        ASSERT(intent.to_state == CoreState::Playing);
        ASSERT(intent.trigger_id == 42);
    });
}

void state_transition_intent_equality(ContractTestRunner& runner) {
    runner.run("state_transition_intent_equality", []() {
        StateTransitionIntent a{CoreState::Idle, CoreState::Playing, 1};
        StateTransitionIntent b{CoreState::Idle, CoreState::Playing, 1};
        StateTransitionIntent c{CoreState::Idle, CoreState::Paused, 1};
        ASSERT(a == b);
        ASSERT(!(a == c));
    });
}

void state_transition_default_construction(ContractTestRunner& runner) {
    runner.run("state_transition_default_construction", []() {
        StateTransition transition{};
        ASSERT(transition.from_state == CoreState::Unknown);
        ASSERT(transition.to_state == CoreState::Unknown);
        ASSERT(transition.timestamp_ns == 0);
        ASSERT(transition.generation_id == 0);
    });
}

void state_transition_field_modification(ContractTestRunner& runner) {
    runner.run("state_transition_field_modification", []() {
        StateTransition transition{};
        transition.from_state = CoreState::Playing;
        transition.to_state = CoreState::Paused;
        transition.timestamp_ns = 1000000;
        transition.generation_id = 5;
        ASSERT(transition.from_state == CoreState::Playing);
        ASSERT(transition.to_state == CoreState::Paused);
        ASSERT(transition.timestamp_ns == 1000000);
        ASSERT(transition.generation_id == 5);
    });
}

void state_transition_equality(ContractTestRunner& runner) {
    runner.run("state_transition_equality", []() {
        StateTransition a{CoreState::Playing, CoreState::Paused, 1000, 1};
        StateTransition b{CoreState::Playing, CoreState::Paused, 1000, 1};
        StateTransition c{CoreState::Playing, CoreState::Paused, 2000, 1};
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

void state_transition_validity_valid(ContractTestRunner& runner) {
    runner.run("state_transition_validity_valid", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionValidity::Valid) == 1);
    });
}

void state_transition_validity_invalid_from_state(ContractTestRunner& runner) {
    runner.run("state_transition_validity_invalid_from_state", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionValidity::InvalidFromState) == 2);
    });
}

void state_transition_validity_invalid_to_state(ContractTestRunner& runner) {
    runner.run("state_transition_validity_invalid_to_state", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionValidity::InvalidToState) == 3);
    });
}

void state_transition_validity_invalid_trigger(ContractTestRunner& runner) {
    runner.run("state_transition_validity_invalid_trigger", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionValidity::InvalidTrigger) == 4);
    });
}

void state_transition_validity_invalid_generation(ContractTestRunner& runner) {
    runner.run("state_transition_validity_invalid_generation", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionValidity::InvalidGeneration) == 5);
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

void state_transition_decision_conditional(ContractTestRunner& runner) {
    runner.run("state_transition_decision_conditional", []() {
        ASSERT(static_cast<uint8_t>(StateTransitionDecision::Conditional) == 6);
    });
}

} // namespace

void run_state_transition_tests(ContractTestRunner& runner) {
    state_transition_intent_default_construction(runner);
    state_transition_intent_field_modification(runner);
    state_transition_intent_equality(runner);
    state_transition_default_construction(runner);
    state_transition_field_modification(runner);
    state_transition_equality(runner);
    state_transition_validity_default_is_unknown(runner);
    state_transition_validity_valid(runner);
    state_transition_validity_invalid_from_state(runner);
    state_transition_validity_invalid_to_state(runner);
    state_transition_validity_invalid_trigger(runner);
    state_transition_validity_invalid_generation(runner);
    state_transition_decision_default_is_unknown(runner);
    state_transition_decision_allow(runner);
    state_transition_decision_reject(runner);
    state_transition_decision_queue(runner);
    state_transition_decision_merge(runner);
    state_transition_decision_defer(runner);
    state_transition_decision_conditional(runner);
}