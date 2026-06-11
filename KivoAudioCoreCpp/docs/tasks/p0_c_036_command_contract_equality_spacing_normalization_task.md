# P0-C-036 Command Contract Equality Spacing Normalization — Taskbook

**Task ID:** `KIVO-AUDIO-CORE-P0-C-036-COMMAND-CONTRACT-EQUALITY-SPACING-NORMALIZATION-TASKBOOK-001`

Implementation allowed by Owner authorization.

## Goal

Normalize the P0-C command aggregate contract headers so the equality operator block is visually separated from the final field block, matching the value-contract readability style already used by neighboring source, policy, observability, error, format, and state contract structs.

This is a formatting-only contract readability cleanup. It must not change fields, values, include boundaries, file tree, tests, CMake registration, genealogy allowlists, or equality declarations.

## Scope

Update only existing production headers under:

```text
include/kivo/core/contract/command/
```

Allowed production headers:

```text
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
- No field type changes.
- No field default changes.
- No include path changes.
- No namespace changes.
- No equality operator signature changes.
- No runtime logic.
- No command dispatcher, queue, scheduler, policy evaluator, source/device mutation, thread, or output path logic.

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

Insert exactly one blank line between the final field declaration and the `[[nodiscard]] constexpr operator==` declaration in the two selected aggregate contract structs.

## Invariants

- Command type count remains unchanged.
- File count remains 2 production headers touched.
- Cross-family direct includes remain unchanged.
- No field names change.
- No field types change.
- No field defaults change.
- No include path changes.
- No namespace changes.
- Existing equality operators remain signature-identical.
- No test source changes.

## Static self-check requirements

After implementation, verify:

```text
selected command aggregate structs have one blank separator before equality operator
selected equality operator declarations remain constexpr and signature-identical
cross-family direct includes unchanged
runtime forbidden tokens = 0
changed files limited to this taskbook + 2 command headers
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-C-036-COMMAND-CONTRACT-EQUALITY-SPACING-NORMALIZATION
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
