# P0-C-026 State Machine Constexpr Equality Normalization — Taskbook

**Task ID:** `KIVO-AUDIO-CORE-P0-C-026-STATE-MACHINE-CONSTEXPR-EQUALITY-NORMALIZATION-TASKBOOK-001`

Implementation allowed by Owner authorization.

## Goal

Normalize the P0-C-011 state machine aggregate contract so its equality operator is explicitly `constexpr`, matching the pure value-contract style now used by neighboring policy, observability, error, source, command, CUE, identity, and generation families.

The state family is already value-only. This task only annotates the existing member `operator==` declaration. It must not change fields, values, include boundaries, file tree, tests, CMake registration, or genealogy allowlists.

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
- No field default changes.
- No include path changes.
- No namespace changes.
- No runtime logic.
- No state machine implementation, transition evaluator, scheduler, dispatcher, command executor, recovery executor, platform, thread, or source/device mutation logic.

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

For `StateMachineContract`, replace:

```cpp
[[nodiscard]] bool operator==(const StateMachineContract&) const noexcept = default;
```

with:

```cpp
[[nodiscard]] constexpr bool operator==(const StateMachineContract&) const noexcept = default;
```

## Invariants

- State type count remains unchanged.
- File count remains 1 production header touched.
- Cross-family direct includes remain unchanged.
- No field names change.
- No field defaults change.
- No include path changes.
- No namespace changes.
- No test source changes.

## Static self-check requirements

After implementation, verify:

```text
StateMachineContract contains [[nodiscard]] constexpr member operator==
non-constexpr member operator== removed from StateMachineContract
cross-family direct includes unchanged
runtime forbidden tokens = 0
changed files limited to this taskbook + state_machine_contract.hpp
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-C-026-STATE-MACHINE-CONSTEXPR-EQUALITY-NORMALIZATION
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
