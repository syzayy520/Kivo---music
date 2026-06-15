# P0-J-009 Runtime EOS Drain Orchestration

**Task:** KIVO-CPP-AUDIO-CORE-P0-J-009-RUNTIME-EOS-DRAIN
**Status:** COMPLETED
**Execution Basis:** P0-J EOS and drain completion
**Last Updated:** 2026-06-12

## Purpose

Connect the terminal render block to the Session drain lifecycle and renderer
drain boundary so a decoded source reaches a truthful terminal playback state.

## Natural Family Tree

```text
src/core/playback/runtime/
  drain/
    playback_runtime_drain.cpp

tests/playback_runtime/coordinator/
  scenario/
    eos_drain_tests.cpp
```

Existing Runtime request, state, step, snapshot, runner, CMake, and genealogy
files may be updated only to configure, invoke, observe, or register this
family.

## Responsibility Rules

- `step/` owns queue pumping and EOS detection only.
- `drain/` owns the EOS drain transaction and result mapping only.
- Open request owns the renderer drain timeout supplied for that Runtime.
- Runtime state stores the active drain request and bounded counters only.
- Runtime snapshot exposes drain observability only.
- EOS scenario tests own success, timeout, and cleanup assertions only.

## Transaction Order

1. The final terminal block is accepted by the renderer.
2. The rendered position advances through the accepted terminal frames.
3. Session returns a generation-safe drain ticket.
4. Runtime invokes renderer `drain()` with the configured timeout.
5. Renderer success completes the ticket as `Succeeded`.
6. Renderer rejection, failure, or timeout completes the ticket as `Failed`.
7. Runtime reports the original EOS disposition only after both boundaries
   complete successfully.

## Failure Rules

- A missing Session drain ticket is a boundary failure.
- Renderer timeout is returned as `RenderFailure::Timeout`.
- Any unsuccessful renderer drain transitions Session to `Failed`.
- Failed drain does not discard ownership before an explicit close or
  shutdown.
- Close after a timeout must close decoder and renderer and deactivate the
  Runtime.
- A second render step after terminal completion is rejected by Session state.

## Non-Goals

- No asynchronous renderer drain.
- No cross-thread cancellation of a blocking platform drain.
- No gapless next-track transition.
- No device-lost recovery policy changes.
- No generic helper, utility, detail, common, or miscellaneous directory.

## Gates

- Runtime `drain/` is an explicit allowed subfamily.
- Runtime roots remain free of directly flattened source files.
- Every Runtime production and test file remains below 260 lines.
- MSVC `/W4 /WX`, all CTest executables, and all cumulative gates pass.

## Exit Criteria

- EOS automatically drains buffered renderer frames.
- Successful EOS leaves Session and renderer in `Stopped`.
- Drain timeout is distinguishable and observable.
- Close after failed drain releases every owned boundary.
- Existing seek/generation tests reflect terminal drain semantics.

## Completion Evidence

- Terminal render acceptance now advances position before starting drain.
- Runtime validates both renderer success and terminal `Stopped` state.
- Drain timeout returns `RenderFailure::Timeout`, transitions Session to
  `Failed`, and remains explicitly closable.
- Close after timeout closes decoder and renderer and clears active Runtime
  ownership.
- Playback Runtime tests passed 6 of 6 scenarios.
- Playback Session tests remained 17 of 17 scenarios.
- Maximum Runtime production file size is 140 lines.
- Maximum Runtime test file size is 152 lines.
- MSVC Debug build passed with `/W4 /WX`.
- CTest passed 11 of 11 test executables.
- All 9 cumulative audio-core gates passed.
