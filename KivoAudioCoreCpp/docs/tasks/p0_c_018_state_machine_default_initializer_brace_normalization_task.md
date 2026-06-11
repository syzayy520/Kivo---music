# P0-C-018 State Machine Default Initializer Brace Normalization — Taskbook

**Task ID:** `KIVO-AUDIO-CORE-P0-C-018-STATE-MACHINE-DEFAULT-INITIALIZER-BRACE-NORMALIZATION-TASKBOOK-001`

Implementation allowed by Owner authorization.

## Goal

Normalize the P0-C-011 state machine aggregate contract so enum default member initializers use brace form consistently with the current value-contract style.

The state family is already value-only. This task only rewrites enum field initializers in `StateMachineContract` from assignment form to brace form. It must not change fields, enum values, include boundaries, file tree, tests, CMake registration, or genealogy allowlists.

## Scope

Update only this existing production header:

```text
include/kivo/core/contract/state/contract/state_machine_contract.hpp
```

## Non-goals

- No new production headers.
- No new tests.
- No CMake changes.
- No runner changes.
- No genealogy gate changes.
- No enum value changes.
- No field name changes.
- No operator changes.
- No comment removal.
- No runtime logic.
- No platform, source, device, command, recovery execution, state machine implementation, transition evaluator, or thread logic.

## Natural family tree

Existing tree is retained exactly:

```text
state/
  phase/
    core_state.hpp
  transition/
    state_transition.hpp
    state_transition_intent.hpp
    state_transition_validity.hpp
    state_transition_decision.hpp
  rule/
    reentrant_transition_policy.hpp
    transition_preemption_policy.hpp
    terminal_state_rule.hpp
  scenario/
    seeking_reentry_rule.hpp
    recovering_seek_rule.hpp
    draining_pause_rule.hpp
    failed_close_rule.hpp
    closed_mutation_rule.hpp
    device_lost_during_seek_rule.hpp
    shutdown_during_drain_rule.hpp
  contract/
    state_machine_contract.hpp
```

## Required transformation

Replace assignment-form default member initializers:

```cpp
ReentrantTransitionPolicy reentrant_transition = ReentrantTransitionPolicy::Unknown;
```

with brace-form equivalents:

```cpp
ReentrantTransitionPolicy reentrant_transition{ReentrantTransitionPolicy::Unknown};
```

Apply to all 10 fields in `StateMachineContract`.

## Invariants

- State type count remains unchanged.
- File count remains 1 production header touched.
- Cross-family direct includes remain 0 beyond existing state-family internal includes.
- No field names change.
- No field defaults change.
- No namespace changes.
- Operator declaration remains unchanged.
- No test source changes.

## Static self-check requirements

After implementation, verify:

```text
assignment-form default initializers removed from StateMachineContract
brace-form default initializers present for all 10 fields
cross-family direct includes = 0 beyond existing state-family internal includes
runtime forbidden tokens = 0
changed files limited to this taskbook + state_machine_contract.hpp
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-C-018-STATE-MACHINE-DEFAULT-INITIALIZER-BRACE-NORMALIZATION
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
