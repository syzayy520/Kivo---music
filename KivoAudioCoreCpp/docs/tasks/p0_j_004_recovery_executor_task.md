# P0-J-004 Recovery Executor

**Task:** KIVO-CPP-AUDIO-CORE-P0-J-004-RECOVERY-EXECUTOR
**Status:** COMPLETED
**Execution Basis:** `docs/architecture/audio_core_execution_roadmap.md`
**Last Updated:** 2026-06-12

## Purpose

Turn recovery classification into bounded executable behavior without placing
WASAPI, FFmpeg, filesystem, or host types in the playback core.

## Natural Family Tree

```text
include/kivo/core/playback/recovery/
  playback_recovery_completion.hpp
  playback_recovery_operations.hpp
  playback_recovery_execution_result.hpp
  playback_recovery_executor.hpp

src/core/playback/recovery/
  playback_recovery_executor.cpp
```

## Rules

- The executor first asks the session controller to enter recovery.
- Wrong-session and non-recoverable requests execute no operation.
- The primary action executes at most once.
- The fallback action executes at most once and only after primary failure.
- Completion explicitly resolves to restored, ready, stopped, or failed.
- A safe stop is a successful bounded recovery outcome, not resumed playback.
- Platform and dependency operations are injected behind a neutral interface.

## Exit Criteria

- Primary success does not execute fallback.
- Primary failure executes exactly one fallback.
- Stop-media fallback produces Stopped.
- Double failure produces Failed.
- Wrong-generation and non-recoverable requests perform no operations.
- Full cumulative validation passes.
