# P0-C-017 Error Default Initializer Brace Normalization — Taskbook

**Task ID:** `KIVO-AUDIO-CORE-P0-C-017-ERROR-DEFAULT-INITIALIZER-BRACE-NORMALIZATION-TASKBOOK-001`

Implementation allowed by Owner authorization.

## Goal

Normalize the P0-C-010 error value structs so their default member initializers use brace form consistently with the surrounding value-contract style.

The error family is already value-only. This task only rewrites enum and bool field initializers from assignment form to brace form. It must not change fields, values, include boundaries, file tree, tests, CMake registration, or genealogy allowlists.

## Scope

Update only existing production headers under:

```text
include/kivo/core/contract/error/
```

Allowed production headers:

```text
include/kivo/core/contract/error/outcome/error_outcome.hpp
include/kivo/core/contract/error/recovery/recovery_matrix.hpp
include/kivo/core/contract/error/severity/error_attributes.hpp
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
- No runtime logic.
- No platform, source, device, command, state, logger, metrics, or recovery execution logic.

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

For the three error structs, replace assignment-form default member initializers:

```cpp
ErrorDomain domain = ErrorDomain::Unknown;
bool recoverable = false;
RecoveryAction primary_action = RecoveryAction::None;
```

with brace-form equivalents:

```cpp
ErrorDomain domain{ErrorDomain::Unknown};
bool recoverable{false};
RecoveryAction primary_action{RecoveryAction::None};
```

## Invariants

- Error type count remains unchanged.
- File count remains 3 production headers touched.
- Cross-family direct includes remain 0 beyond existing error-family internal includes.
- No field names change.
- No field defaults change.
- No namespace changes.
- No operator declarations change.
- No test source changes.

## Static self-check requirements

After implementation, verify:

```text
assignment-form default initializers removed from the 3 error structs
brace-form default initializers present for all fields in those structs
cross-family direct includes = 0 beyond existing error-family internal includes
runtime forbidden tokens = 0
changed files limited to this taskbook + 3 error headers
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-C-017-ERROR-DEFAULT-INITIALIZER-BRACE-NORMALIZATION
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
