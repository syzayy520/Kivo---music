# P0-C-021 Source Boundary Constexpr Equality Normalization — Taskbook

**Task ID:** `KIVO-AUDIO-CORE-P0-C-021-SOURCE-BOUNDARY-CONSTEXPR-EQUALITY-NORMALIZATION-TASKBOOK-001`

Implementation allowed by Owner authorization.

## Goal

Normalize selected P0-C-008 source aggregate value structs so equality operators are explicitly `constexpr`, matching the pure value-token style already used by source identity wrappers and generation contract families.

The source family is already value-only. This task only annotates existing member `operator==` declarations. It must not change fields, values, include boundaries, file tree, tests, CMake registration, or genealogy allowlists.

## Scope

Update only existing production headers under:

```text
include/kivo/core/contract/source/
```

Allowed production headers:

```text
include/kivo/core/contract/source/reader/reader_boundary_contract.hpp
include/kivo/core/contract/source/container/container_boundary_contract.hpp
include/kivo/core/contract/source/boundary/source_boundary_contract.hpp
include/kivo/core/contract/source/outcome/source_outcome.hpp
```

## Non-goals

- No new production headers.
- No new tests.
- No CMake changes.
- No runner changes.
- No genealogy gate changes.
- No field name changes.
- No field default changes.
- No include path changes.
- No namespace changes.
- No runtime logic.
- No source reader implementation, file IO, network IO, parser, scanner, decoder, scheduler, thread, or recovery execution logic.

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
    source_location_ref.hpp
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

For the four selected source structs, replace:

```cpp
[[nodiscard]] bool operator==(const Type&) const noexcept = default;
```

with:

```cpp
[[nodiscard]] constexpr bool operator==(const Type&) const noexcept = default;
```

## Invariants

- Source type count remains unchanged.
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
all 4 selected source structs contain [[nodiscard]] constexpr member operator==
non-constexpr member operator== removed from those 4 structs
cross-family direct includes unchanged
runtime forbidden tokens = 0
changed files limited to this taskbook + 4 source headers
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-C-021-SOURCE-BOUNDARY-CONSTEXPR-EQUALITY-NORMALIZATION
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
