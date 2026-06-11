# P0-C-023 Error Constexpr Equality Normalization — Taskbook

**Task ID:** `KIVO-AUDIO-CORE-P0-C-023-ERROR-CONSTEXPR-EQUALITY-NORMALIZATION-TASKBOOK-001`

Implementation allowed by Owner authorization.

## Goal

Normalize selected P0-C-010 error aggregate value structs so equality operators are explicitly `constexpr`, matching the pure value-contract style now used by neighboring source, command, CUE, identity, and generation families.

The error family is already value-only. This task only annotates existing member `operator==` declarations. It must not change fields, values, include boundaries, file tree, tests, CMake registration, or genealogy allowlists.

## Scope

Update only existing production headers under:

```text
include/kivo/core/contract/error/
```

Allowed production headers:

```text
include/kivo/core/contract/error/severity/error_attributes.hpp
include/kivo/core/contract/error/recovery/recovery_matrix.hpp
include/kivo/core/contract/error/outcome/error_outcome.hpp
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
- No recovery executor, logger, telemetry sink, platform, source reader, device reset, command dispatcher, thread, or state mutation logic.

## Natural family tree

Existing tree is retained exactly:

```text
error/
  domain/
    error_domain.hpp
    error_domain_category.hpp
  severity/
    error_severity.hpp
    error_attributes.hpp
    safe_cleanup_path.hpp
  recovery/
    recovery_action.hpp
    recovery_matrix.hpp
  outcome/
    error_outcome.hpp
```

## Required transformation

For the three selected error structs, replace:

```cpp
[[nodiscard]] bool operator==(const Type&) const noexcept = default;
```

with:

```cpp
[[nodiscard]] constexpr bool operator==(const Type&) const noexcept = default;
```

## Invariants

- Error type count remains unchanged.
- File count remains 3 production headers touched.
- Cross-family direct includes remain unchanged.
- No field names change.
- No field defaults change.
- No include path changes.
- No namespace changes.
- No test source changes.

## Static self-check requirements

After implementation, verify:

```text
all 3 selected error structs contain [[nodiscard]] constexpr member operator==
non-constexpr member operator== removed from those 3 structs
cross-family direct includes unchanged
runtime forbidden tokens = 0
changed files limited to this taskbook + 3 error headers
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-C-023-ERROR-CONSTEXPR-EQUALITY-NORMALIZATION
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
