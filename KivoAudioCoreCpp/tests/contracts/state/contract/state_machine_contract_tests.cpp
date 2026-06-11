#include "../../contract_tests_main.h"
#include "kivo/core/contract/state/contract/state_machine_contract.hpp"

using namespace kivo::core::contract;

namespace {

void state_machine_contract_default_construction(ContractTestRunner& runner) {
    runner.run("state_machine_contract_default_construction", []() {
        StateMachineContract c{};
        ASSERT(c.reentrant_transition == ReentrantTransitionPolicy::Unknown);
        ASSERT(c.transition_preemption == TransitionPreemptionPolicy::Unknown);
        ASSERT(c.terminal_state == TerminalStateRule::Unknown);
        ASSERT(c.seeking_reentry == SeekingReentryRule::Unknown);
        ASSERT(c.recovering_seek == RecoveringSeekRule::Unknown);
        ASSERT(c.draining_pause == DrainingPauseRule::Unknown);
        ASSERT(c.failed_close == FailedCloseRule::Unknown);
        ASSERT(c.closed_mutation == ClosedMutationRule::Unknown);
        ASSERT(c.device_lost_during_seek == DeviceLostDuringSeekRule::Unknown);
        ASSERT(c.shutdown_during_drain == ShutdownDuringDrainRule::Unknown);
    });
}

void state_machine_contract_field_modification(ContractTestRunner& runner) {
    runner.run("state_machine_contract_field_modification", []() {
        StateMachineContract c{};
        c.reentrant_transition = ReentrantTransitionPolicy::Merge;
        c.transition_preemption = TransitionPreemptionPolicy::NeverPreempt;
        c.terminal_state = TerminalStateRule::AllowCloseOnly;
        c.seeking_reentry = SeekingReentryRule::QueueSeek;
        c.recovering_seek = RecoveringSeekRule::DeferUntilRecovered;
        c.draining_pause = DrainingPauseRule::CompleteDrainFirst;
        c.failed_close = FailedCloseRule::CleanupThenClose;
        c.closed_mutation = ClosedMutationRule::RejectMutation;
        c.device_lost_during_seek = DeviceLostDuringSeekRule::EnterRecovering;
        c.shutdown_during_drain = ShutdownDuringDrainRule::ForceClose;

        ASSERT(c.reentrant_transition == ReentrantTransitionPolicy::Merge);
        ASSERT(c.transition_preemption == TransitionPreemptionPolicy::NeverPreempt);
        ASSERT(c.terminal_state == TerminalStateRule::AllowCloseOnly);
        ASSERT(c.seeking_reentry == SeekingReentryRule::QueueSeek);
        ASSERT(c.recovering_seek == RecoveringSeekRule::DeferUntilRecovered);
        ASSERT(c.draining_pause == DrainingPauseRule::CompleteDrainFirst);
        ASSERT(c.failed_close == FailedCloseRule::CleanupThenClose);
        ASSERT(c.closed_mutation == ClosedMutationRule::RejectMutation);
        ASSERT(c.device_lost_during_seek == DeviceLostDuringSeekRule::EnterRecovering);
        ASSERT(c.shutdown_during_drain == ShutdownDuringDrainRule::ForceClose);
    });
}

void state_machine_contract_equality(ContractTestRunner& runner) {
    runner.run("state_machine_contract_equality", []() {
        StateMachineContract a{};
        StateMachineContract b{};
        ASSERT(a == b);

        StateMachineContract c{};
        c.reentrant_transition = ReentrantTransitionPolicy::Merge;
        ASSERT(!(a == c));
    });
}

void state_machine_contract_has_all_required_fields(ContractTestRunner& runner) {
    runner.run("state_machine_contract_has_all_required_fields", []() {
        StateMachineContract c{};
        // Rule fields (3)
        (void)c.reentrant_transition;
        (void)c.transition_preemption;
        (void)c.terminal_state;
        // Scenario fields (7)
        (void)c.seeking_reentry;
        (void)c.recovering_seek;
        (void)c.draining_pause;
        (void)c.failed_close;
        (void)c.closed_mutation;
        (void)c.device_lost_during_seek;
        (void)c.shutdown_during_drain;
        ASSERT(true);
    });
}

} // namespace

void run_state_machine_contract_tests(ContractTestRunner& runner) {
    state_machine_contract_default_construction(runner);
    state_machine_contract_field_modification(runner);
    state_machine_contract_equality(runner);
    state_machine_contract_has_all_required_fields(runner);
}
