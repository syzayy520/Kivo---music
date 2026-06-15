# P0-J-011 Recovery Family Hardening

**Task:** KIVO-CPP-AUDIO-CORE-P0-J-011-RECOVERY-FAMILY-HARDENING
**Status:** COMPLETED
**Execution Basis:** Runtime genealogy iron rule
**Last Updated:** 2026-06-12

## Purpose

Remove the inherited flat playback Recovery family before adding decode/source
failure classification.

## Natural Family Tree

```text
include/kivo/core/playback/recovery/
  completion/
    playback_recovery_completion.hpp
  execution/
    playback_recovery_execution_result.hpp
  executor/
    playback_recovery_executor.hpp
  operations/
    playback_recovery_operations.hpp
  policy/
    playback_recovery_policy.hpp

src/core/playback/recovery/
  executor/
    playback_recovery_executor.cpp
```

## Responsibility Rules

- Completion owns terminal recovery vocabulary only.
- Execution owns executor outcome observation only.
- Executor owns bounded primary/fallback orchestration only.
- Operations owns the injected platform-neutral operation boundary only.
- Policy owns constexpr error-domain classification only.
- No file changes behavior during this task.

## Non-Goals

- No new error mapping.
- No new recovery action.
- No Runtime behavior change.
- No source or codec rebuild implementation.
- No generic helper, utility, detail, common, or miscellaneous directory.

## Gates

- Recovery public and source roots may not contain directly flattened files.
- Immediate subfamilies must exactly match the declared tree.
- Recovery production files must remain below 260 lines.
- All moved public headers must compile independently.
- MSVC `/W4 /WX`, all CTest executables, and all cumulative gates pass.

## Exit Criteria

- Every Recovery file has one explicit owning subfamily.
- Old flat include and source paths have zero references.
- Existing Session and Runtime recovery behavior remains unchanged.

## Completion Evidence

- Public and source Recovery roots contain zero direct files.
- Old flat include and source paths have zero references.
- Largest Recovery production file is 60 lines.
- MSVC Debug build passes with `/W4 /WX`.
- CTest passes 11/11 executables, including Session 17/17 and Runtime 8/8.
- Cumulative audio-core gates pass 9/9.
