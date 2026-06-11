# P0-C-039 Source Generation Observer Order Normalization — Taskbook

**Task ID:** `KIVO-AUDIO-CORE-P0-C-039-SOURCE-GENERATION-OBSERVER-ORDER-NORMALIZATION-TASKBOOK-001`

Implementation allowed by Owner authorization.

## Goal

Normalize the P0-C `SourceGeneration` value wrapper so its observer is placed before its equality operator, matching the local style used by `GenerationId` and the wider value-contract layout: fields, observers/convenience methods, then comparison.

This is a declaration-order readability cleanup. It must not change fields, values, include boundaries, file tree, tests, CMake registration, genealogy allowlists, observer behavior, or equality behavior.

## Scope

Update only this existing production header:

```text
include/kivo/core/contract/source/identity/source_generation.hpp
```

## Non-goals

- No new production headers.
- No new tests.
- No CMake changes.
- No runner changes.
- No genealogy gate changes.
- No field name changes.
- No field type changes.
- No field default changes.
- No include path changes.
- No namespace changes.
- No observer signature changes.
- No equality operator signature changes.
- No runtime logic.
- No source reader, source locator, boundary adapter, runtime source mutation, thread, or output path logic.

## Natural family tree

Existing tree is retained exactly:

```text
source/
  identity/
    source_identity.hpp
    source_generation.hpp
  location/
    source_uri.hpp
    source_path.hpp
    source_locator.hpp
  access/
    source_access_type.hpp
    source_readiness.hpp
    source_ownership_policy.hpp
    source_fault_policy.hpp
  reader/
    reader_identity.hpp
    reader_capability.hpp
    reader_failure.hpp
    reader_boundary_contract.hpp
  container/
    container_identity.hpp
    container_capability.hpp
    container_failure.hpp
    container_boundary_contract.hpp
  boundary/
    source_boundary_mode.hpp
    source_boundary_contract.hpp
  outcome/
    source_outcome_reason.hpp
    source_recovery_policy.hpp
    source_outcome.hpp
```

## Required transformation

Move the existing observer:

```cpp
[[nodiscard]] constexpr uint64_t value() const noexcept { return id.value(); }
```

above the existing equality operator:

```cpp
[[nodiscard]] constexpr bool operator==(const SourceGeneration&) const noexcept = default;
```

## Invariants

- Source type count remains unchanged.
- File count remains 1 production header touched.
- Cross-family direct includes remain unchanged.
- No field names change.
- No field types change.
- No field defaults change.
- No include path changes.
- No namespace changes.
- Observer signature and body remain unchanged.
- Equality operator signature remains unchanged.
- No test source changes.

## Static self-check requirements

After implementation, verify:

```text
SourceGeneration observer appears before comparison declaration
SourceGeneration observer signature/body unchanged
SourceGeneration equality declaration unchanged
cross-family direct includes unchanged
runtime forbidden tokens = 0
changed files limited to this taskbook + source_generation.hpp
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-C-039-SOURCE-GENERATION-OBSERVER-ORDER-NORMALIZATION
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
