# P0-D Validation Result Classification Guide

## Purpose

P0-D validation must distinguish source-code failures from local environment blockers.

A commercial-grade audio core cannot treat missing local tools as source failure, and it cannot treat source failures as harmless environment noise.

## PASS classifications

Use PASS only when the requested validation actually ran and succeeded.

```text
PASS_GIT_STATE_CLEAN_HEAD_EQUALS_REMOTE
PASS_TOOLCHAIN_PROBE_READY_NINJA
PASS_TOOLCHAIN_PROBE_READY_COMPILER_NO_NINJA
PASS_BUILD_VALIDATION_NINJA
PASS_BUILD_VALIDATION_VISUAL_STUDIO
PASS_GATE_VALIDATION
PASS_VALIDATION_BATCH
PASS_CONFIGURE_SMOKE
PASS_VERIFIED_BY_USER_LOCAL_RUN
```

## BLOCKED classifications

Use BLOCKED when validation could not run because required local preconditions were missing.

```text
BLOCKED_BY_LOCAL_TOOLCHAIN
BLOCKED_BY_LOCAL_GENERATOR
BLOCKED_BY_GATE_DETAIL_REQUIRED
BLOCKED_GATE_VALIDATION_ENVIRONMENT
BLOCKED_VALIDATION_BATCH_GATES
```

A BLOCKED result is not a source-code PASS. It means the result is deferred until the missing precondition is fixed.
`PASS_CONFIGURE_SMOKE` is not a build, test, runtime, or release pass.

## FAIL classifications

Use FAIL when the required validation ran and found a source, structure, or gate problem.

```text
FAIL_GIT_WORKTREE_NOT_CLEAN
FAIL_GIT_HEAD_REMOTE_MISMATCH
FAIL_BUILD_VALIDATION_STEP
FAIL_GATE_VALIDATION
FAIL_VALIDATION_BATCH_GIT_PRECONDITION
FAIL_VALIDATION_BATCH_ENVIRONMENT_PRECONDITION
FAIL_VALIDATION_BATCH_BUILD
FAIL_VALIDATION_BATCH_GATES
FAIL_VALIDATION_BATCH_DIFF_CHECK
FAIL_GATE_VALIDATION_UNKNOWN_RESULT
```

## Deferred validation language

Use this when a small contract-only or documentation-only task is complete but full validation cannot run locally:

```text
STATIC_SELF_CHECK_PASS_FULL_VALIDATION_DEFERRED
```

Use this when a task was implemented and pushed, but the local environment blocks build/test validation:

```text
IMPLEMENTED_PUSHED_LOCAL_HEAD_EQUALS_REMOTE
FULL_LOCAL_VALIDATION_DEFERRED_BY_LOCAL_TOOLCHAIN
```

## Stage closeout language

A stage may be marked ready for closeout only when the unresolved items are explicitly classified.

Examples:

```text
P0_C_READY_FOR_CLOSEOUT_DECISION
P0_D_VALIDATION_TOOLING_IN_PROGRESS
READY_FOR_LOCAL_VALIDATION
READY_FOR_USER_LOCAL_VERIFICATION
```

## Invalid shortcuts

Do not claim any of these unless the matching validation actually ran:

```text
BUILD_PASS
TEST_PASS
GATES_PASS
PASS_VERIFIED_BY_USER_LOCAL_RUN
```

Do not treat `No CMAKE_CXX_COMPILER could be found` as a source-code failure. It is a local toolchain blocker until proven otherwise.
Do not report `OVERALL: ENVIRONMENT_BLOCKED` as `OVERALL: PASS`.
