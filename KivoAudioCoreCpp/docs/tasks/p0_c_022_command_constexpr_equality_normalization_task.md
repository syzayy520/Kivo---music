# P0-C-022 Command Constexpr Equality Normalization — Taskbook

**Task ID:** `KIVO-AUDIO-CORE-P0-C-022-COMMAND-CONSTEXPR-EQUALITY-NORMALIZATION-TASKBOOK-001`

Implementation allowed by Owner authorization.

## Goal

Normalize selected P0-C-012 command value structs so equality operators are explicitly `constexpr`, matching the pure value-token style already used by identity and generation contract families.

The command family is already value-only. This task only annotates existing member `operator==` declarations. It must not change fields, values, include boundaries, file tree, tests, CMake registration, or genealogy allowlists.

## Scope

Update only existing production headers under:

```text
include/kivo/core/contract/command/
```

Allowed production headers:

```text
include/kivo/core/contract/command/identity/command_id.hpp
include/kivo/core/contract/command/identity/command_generation_ref.hpp
include/kivo/core/contract/command/ordering/command_ordering_contract.hpp
include/kivo/core/contract/command/contract/command_semantics_contract.hpp
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
- No command queue, scheduler, dispatcher, executor, state mutation, source operation, device operation, thread, or recovery execution logic.

## Natural family tree

Existing tree is retained exactly:

```text
command/
  identity/
    command_id.hpp
    command_generation_ref.hpp
  kind/
    command_kind.hpp
    command_origin.hpp
    command_priority.hpp
  ordering/
    command_ordering_policy.hpp
    command_supersession_policy.hpp
    command_coalescing_policy.hpp
    command_ordering_contract.hpp
  decision/
    command_decision.hpp
    command_rejection_reason.hpp
  scenario/
    rapid_seek_rule.hpp
    rapid_track_switch_rule.hpp
    pause_during_track_switch_rule.hpp
    seek_during_recovery_rule.hpp
    close_during_drain_rule.hpp
    shutdown_during_write_rule.hpp
    library_scan_during_media_rule.hpp
    source_disconnect_during_seek_rule.hpp
    device_lost_during_flush_rule.hpp
  contract/
    command_semantics_contract.hpp
```

## Required transformation

For the four selected command structs, replace:

```cpp
[[nodiscard]] bool operator==(const Type&) const noexcept = default;
```

with:

```cpp
[[nodiscard]] constexpr bool operator==(const Type&) const noexcept = default;
```

## Invariants

- Command type count remains unchanged.
- File count remains 4 production headers touched.
- Cross-family direct includes remain unchanged.
- No field names change.
- No field defaults change.
- No include path changes.
- No namespace changes.
- No test source changes.

## Static self-check requirements

After implementation, verify:

```text
all 4 selected command structs contain [[nodiscard]] constexpr member operator==
non-constexpr member operator== removed from those 4 structs
cross-family direct includes unchanged
runtime forbidden tokens = 0
changed files limited to this taskbook + 4 command headers
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-C-022-COMMAND-CONSTEXPR-EQUALITY-NORMALIZATION
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
