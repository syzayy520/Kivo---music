# P0-C-038 GenerationId Private Default Initializer Brace Normalization — Taskbook

**Task ID:** `KIVO-AUDIO-CORE-P0-C-038-GENERATION-ID-PRIVATE-DEFAULT-INITIALIZER-BRACE-NORMALIZATION-TASKBOOK-001`

Implementation allowed by Owner authorization.

## Goal

Normalize the root `GenerationId` value token so its private integer member uses explicit brace-form default member initialization, matching the value-contract initializer style used by the P0-C contract families.

This is a root shared value-token cleanup. It must not change public factory behavior, observer behavior, comparison operators, include boundaries, file tree, tests, CMake registration, genealogy allowlists, or runtime logic.

## Scope

Update only this existing production header:

```text
include/kivo/core/contract/generation_id.hpp
```

## Non-goals

- No new production headers.
- No new tests.
- No CMake changes.
- No runner changes.
- No genealogy gate changes.
- No field name changes.
- No field type changes.
- No public API changes.
- No factory method changes.
- No observer method changes.
- No comparison operator changes.
- No include path changes.
- No namespace changes.
- No runtime logic.

## Natural family tree

Existing shared root token remains in place:

```text
contract/
  generation_id.hpp
  source/
    identity/
      source_generation.hpp
  ... other contract families that may reference GenerationId
```

This task does not move `GenerationId` into a subfamily and does not create a new shared/helper folder.

## Required transformation

Replace assignment-form private default member initialization:

```cpp
uint64_t id_ = 0;
```

with brace-form default member initialization:

```cpp
uint64_t id_{0};
```

## Invariants

- Constructor behavior remains unchanged.
- `initial()` behavior remains unchanged.
- `next()` behavior remains unchanged.
- `value()` behavior remains unchanged.
- Equality and spaceship declarations remain unchanged.
- Include paths remain unchanged.
- Namespace remains unchanged.
- No test source changes.

## Static self-check requirements

After implementation, verify:

```text
GenerationId private member is initialized with id_{0}
GenerationId factory methods unchanged
GenerationId observer unchanged
GenerationId comparison declarations unchanged
cross-family direct includes unchanged
runtime forbidden tokens = 0
changed files limited to this taskbook + generation_id.hpp
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-C-038-GENERATION-ID-PRIVATE-DEFAULT-INITIALIZER-BRACE-NORMALIZATION
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
