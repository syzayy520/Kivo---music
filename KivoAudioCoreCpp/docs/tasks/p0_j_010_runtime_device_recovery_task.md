# P0-J-010 Runtime Device-Lost Recovery

**Task:** KIVO-CPP-AUDIO-CORE-P0-J-010-RUNTIME-DEVICE-RECOVERY
**Status:** COMPLETED
**Execution Basis:** P0-J device-lost recovery and reopen
**Last Updated:** 2026-06-12

## Purpose

Turn a renderer `DeviceLost` failure into an explicit control-thread recovery
transaction instead of terminating playback immediately.

## Natural Family Tree

```text
src/core/playback/runtime/
  recovery/
    playback_runtime_device_recovery.cpp

tests/playback_runtime/coordinator/
  scenario/
    device_recovery_tests.cpp
```

Existing Runtime coordinator, failure, open, resource, snapshot, state, step,
runner, CMake, and genealogy files may be updated only to detect, execute,
observe, or register this family.

## Responsibility Rules

- `step/` detects renderer device loss and requests Session recovery only.
- `recovery/` owns renderer reopen and pipeline reconstruction only.
- The public coordinator exposes one control-thread `recover()` entry point.
- Runtime state retains the accepted renderer open request and pending recovery
  flag only.
- Runtime snapshot exposes device-loss and recovery counters only.
- Device recovery scenarios contain deterministic success and failure paths.

## Recovery Transaction

1. Render write returns `RenderFailure::DeviceLost`.
2. Session classifies the error and enters `Recovering`.
3. Runtime marks one pending device recovery and returns the original render
   failure.
4. Control code calls `recover()`.
5. Runtime captures the last rendered position and advances decode, seek, and
   flush generations.
6. Runtime discards the old producer and queue contents.
7. Renderer closes and reopens with the accepted format and advanced
   generations.
8. Decoder seeks to the last rendered position with the new decode generation.
9. Runtime recreates the queue producer with renderer-authoritative
   generations.
10. Renderer starts and Session completes recovery as `Restored`.

## Failure Rules

- `recover()` without a pending device recovery is rejected.
- Generation exhaustion fails recovery instead of wrapping to zero.
- Renderer close/open, decoder seek, producer creation, or renderer start
  failure completes Session recovery as `Failed`.
- Failed recovery retains Runtime ownership until explicit close or shutdown.
- Close while recovery is pending cancels ownership and closes both boundaries.
- Stale blocks from the failed device generation cannot survive queue reset.

## Non-Goals

- No automatic recovery inside the render callback.
- No format conversion if a replacement device rejects the active format.
- No source rebuild or codec rebuild.
- No background retry loop.
- No generic helper, utility, detail, common, or miscellaneous directory.

## Gates

- Runtime `recovery/` is an explicit allowed subfamily.
- Runtime roots remain free of directly flattened source files.
- Every Runtime production and test file remains below 260 lines.
- MSVC `/W4 /WX`, all CTest executables, and all cumulative gates pass.

## Exit Criteria

- Device loss enters `Recovering`, not terminal `Failed`.
- Successful recovery reopens renderer, seeks decoder, advances generations,
  resumes playback, and can subsequently reach EOS.
- Failed recovery is classified and can still close cleanly.
- Recovery counters distinguish observed, attempted, successful, and failed
  operations.

## Completion Evidence

- Renderer `DeviceLost` now requests Session recovery without running reopen
  work in the render step.
- Control-thread `recover()` closes and reopens the renderer with exact active
  format, seeks decoder to the last rendered position, resets queued audio,
  advances decode/seek/flush generations, rebuilds the producer, and resumes.
- Generation exhaustion is checked before any generation advances.
- Successful recovery resumed at frame 100 and subsequently drained at frame
  104 in the deterministic test.
- Decoder seek failure completed Session recovery as `Failed`; explicit close
  still released decoder and renderer ownership.
- Playback Runtime tests passed 8 of 8 scenarios.
- Playback Session tests remained 17 of 17 scenarios.
- Maximum Runtime production file size is 153 lines.
- Maximum Runtime test file size is 156 lines.
- MSVC Debug build passed with `/W4 /WX`.
- CTest passed 11 of 11 test executables.
- All 9 cumulative audio-core gates passed.
