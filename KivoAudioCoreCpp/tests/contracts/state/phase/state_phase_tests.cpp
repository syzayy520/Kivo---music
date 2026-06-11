#include "../../contract_tests_main.h"
#include "kivo/core/contract/state/phase/core_state.hpp"
#include "kivo/core/contract/state/phase/state_stability.hpp"
#include "kivo/core/contract/state/phase/state_terminality.hpp"

using namespace kivo::core::contract;

namespace {

void core_state_default_is_unknown(ContractTestRunner& runner) {
    runner.run("core_state_default_is_unknown", []() {
        CoreState s{};
        ASSERT(s == CoreState::Unknown);
    });
}

void core_state_idle(ContractTestRunner& runner) {
    runner.run("core_state_idle", []() {
        ASSERT(static_cast<uint8_t>(CoreState::Idle) == 1);
    });
}

void core_state_opening(ContractTestRunner& runner) {
    runner.run("core_state_opening", []() {
        ASSERT(static_cast<uint8_t>(CoreState::Opening) == 2);
    });
}

void core_state_ready(ContractTestRunner& runner) {
    runner.run("core_state_ready", []() {
        ASSERT(static_cast<uint8_t>(CoreState::Ready) == 3);
    });
}

void core_state_playing(ContractTestRunner& runner) {
    runner.run("core_state_playing", []() {
        ASSERT(static_cast<uint8_t>(CoreState::Playing) == 4);
    });
}

void core_state_pausing(ContractTestRunner& runner) {
    runner.run("core_state_pausing", []() {
        ASSERT(static_cast<uint8_t>(CoreState::Pausing) == 5);
    });
}

void core_state_paused(ContractTestRunner& runner) {
    runner.run("core_state_paused", []() {
        ASSERT(static_cast<uint8_t>(CoreState::Paused) == 6);
    });
}

void core_state_seeking(ContractTestRunner& runner) {
    runner.run("core_state_seeking", []() {
        ASSERT(static_cast<uint8_t>(CoreState::Seeking) == 7);
    });
}

void core_state_draining(ContractTestRunner& runner) {
    runner.run("core_state_draining", []() {
        ASSERT(static_cast<uint8_t>(CoreState::Draining) == 8);
    });
}

void core_state_recovering(ContractTestRunner& runner) {
    runner.run("core_state_recovering", []() {
        ASSERT(static_cast<uint8_t>(CoreState::Recovering) == 9);
    });
}

void core_state_stopped(ContractTestRunner& runner) {
    runner.run("core_state_stopped", []() {
        ASSERT(static_cast<uint8_t>(CoreState::Stopped) == 10);
    });
}

void core_state_failed(ContractTestRunner& runner) {
    runner.run("core_state_failed", []() {
        ASSERT(static_cast<uint8_t>(CoreState::Failed) == 11);
    });
}

void core_state_closed(ContractTestRunner& runner) {
    runner.run("core_state_closed", []() {
        ASSERT(static_cast<uint8_t>(CoreState::Closed) == 12);
    });
}

void core_state_equality(ContractTestRunner& runner) {
    runner.run("core_state_equality", []() {
        ASSERT(CoreState::Playing == CoreState::Playing);
        ASSERT(CoreState::Unknown != CoreState::Playing);
    });
}

void state_stability_default_is_unknown(ContractTestRunner& runner) {
    runner.run("state_stability_default_is_unknown", []() {
        StateStability s{};
        ASSERT(s == StateStability::Unknown);
    });
}

void state_stability_stable(ContractTestRunner& runner) {
    runner.run("state_stability_stable", []() {
        ASSERT(static_cast<uint8_t>(StateStability::Stable) == 1);
    });
}

void state_stability_transitioning(ContractTestRunner& runner) {
    runner.run("state_stability_transitioning", []() {
        ASSERT(static_cast<uint8_t>(StateStability::Transitioning) == 2);
    });
}

void state_stability_unstable(ContractTestRunner& runner) {
    runner.run("state_stability_unstable", []() {
        ASSERT(static_cast<uint8_t>(StateStability::Unstable) == 3);
    });
}

void state_terminality_default_is_unknown(ContractTestRunner& runner) {
    runner.run("state_terminality_default_is_unknown", []() {
        StateTerminality t{};
        ASSERT(t == StateTerminality::Unknown);
    });
}

void state_terminality_non_terminal(ContractTestRunner& runner) {
    runner.run("state_terminality_non_terminal", []() {
        ASSERT(static_cast<uint8_t>(StateTerminality::NonTerminal) == 1);
    });
}

void state_terminality_terminal(ContractTestRunner& runner) {
    runner.run("state_terminality_terminal", []() {
        ASSERT(static_cast<uint8_t>(StateTerminality::Terminal) == 2);
    });
}

} // namespace

void run_state_phase_tests(ContractTestRunner& runner) {
    core_state_default_is_unknown(runner);
    core_state_idle(runner);
    core_state_opening(runner);
    core_state_ready(runner);
    core_state_playing(runner);
    core_state_pausing(runner);
    core_state_paused(runner);
    core_state_seeking(runner);
    core_state_draining(runner);
    core_state_recovering(runner);
    core_state_stopped(runner);
    core_state_failed(runner);
    core_state_closed(runner);
    core_state_equality(runner);
    state_stability_default_is_unknown(runner);
    state_stability_stable(runner);
    state_stability_transitioning(runner);
    state_stability_unstable(runner);
    state_terminality_default_is_unknown(runner);
    state_terminality_non_terminal(runner);
    state_terminality_terminal(runner);
}
