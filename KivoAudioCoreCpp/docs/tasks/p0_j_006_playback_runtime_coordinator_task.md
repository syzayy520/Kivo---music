# P0-J-006 Playback Runtime Coordinator

**Task:** KIVO-CPP-AUDIO-CORE-P0-J-006-PLAYBACK-RUNTIME-COORDINATOR
**Status:** COMPLETED
**Execution Basis:** `docs/architecture/audio_core_execution_roadmap.md`
**Last Updated:** 2026-06-12

## Purpose

Create one platform-neutral runtime owner that applies serialized session
commands to decode, queue, and render boundaries as a consistent transaction.
The coordinator must prevent stale commands and stale audio generations from
mutating the active session.

## Scope

- Transactional source open with deterministic cleanup on every failed stage.
- Resume, pause, stop, flush, close, and shutdown orchestration.
- Seek-scoped decoder, queue, renderer, generation, and position reset.
- Step-driven production and rendering for deterministic race tests.
- Explicit failure classification without platform types crossing the core.

## Non-Goals

- No internal worker thread.
- No device-selection policy.
- No recovery retry loop in this slice.
- No gapless scheduling or processing chain.
- No generic helper, utility, common, manager, or miscellaneous family.

## Natural Family Tree

```text
include/kivo/core/playback/runtime/
  coordinator/
    playback_runtime_coordinator.hpp
  failure/
    playback_runtime_failure.hpp
  request/
    playback_runtime_open_request.hpp
  result/
    playback_runtime_result.hpp
  snapshot/
    playback_runtime_snapshot.hpp

src/core/playback/runtime/
  command/
    playback_runtime_command_dispatch.cpp
    playback_runtime_command_validation.cpp
  coordinator/
    playback_runtime_coordinator.cpp
  flush/
    playback_runtime_flush.cpp
  lifecycle/
    playback_runtime_lifecycle.cpp
  open/
    playback_runtime_open.cpp
  resource/
    playback_runtime_resource.cpp
  result/
    playback_runtime_result_factory.hpp
  seek/
    playback_runtime_seek.cpp
  snapshot/
    playback_runtime_snapshot.cpp
  state/
    playback_runtime_coordinator_state.hpp
    playback_runtime_coordinator_state.cpp
  step/
    playback_runtime_step.cpp
  transport/
    playback_runtime_transport.cpp

tests/playback_runtime/
  coordinator/
    fixture/
      runtime_test_assert.hpp
      runtime_test_byte_source.hpp
      runtime_test_decoder.hpp
      runtime_test_factory.hpp
    runner/
      playback_runtime_test_cases.hpp
      playback_runtime_tests_main.cpp
    scenario/
      command_isolation_tests.cpp
      lifecycle_tests.cpp
      open_transaction_tests.cpp
      seek_generation_tests.cpp
```

`runtime/` is a genealogy root and may not directly contain source files.
Every immediate child is one responsibility family. A new child family is
allowed only when it has an immediate production consumer and same-slice test.

## Responsibility Table

| Family | Sole responsibility |
|---|---|
| `coordinator/` | Serialize and apply cross-boundary runtime transactions |
| `request/` | Describe immutable runtime-open input |
| `failure/` | Classify coordinator-owned failures |
| `result/` | Report one operation outcome and boundary failures |
| `snapshot/` | Expose bounded runtime observation state |

## Include Rules

- `coordinator` may depend on all four runtime value families and existing
  decode/render/session boundaries.
- Value families may depend on existing contracts but never on `coordinator`.
- No runtime family may include FFmpeg, WASAPI, Windows, COM, or test headers.
- Tests may depend on the coordinator and deterministic fake boundaries.

## Transaction Rules

1. Validate command ordering and generation before mutating boundaries.
2. Stage queue memory before releasing an active source.
3. Open renderer, open decoder, create producer, then commit the session.
4. Close every successfully opened boundary if a later open stage fails.
5. Resume and pause roll renderer state back when session commit is rejected.
6. Seek advances decode, seek, and flush generations exactly once.
7. Seek resets the queue only while producer and consumer are quiescent.
8. Old source or decode generations fail before queue mutation.
9. Close and shutdown release runtime resources before final state commit.

## Gate Rules

- `check_genealogy_gate.ps1` rejects files directly under any runtime family
  root and rejects undeclared immediate subfamilies.
- Runtime production and test source files exceeding 260 lines fail the
  responsibility-overflow check and must be split by an actual change axis.
- Runtime paths and CMake targets must be explicitly authorized by the active
  runtime and forbidden-token gates.
- Public-header composition, `/W4 /WX`, deterministic tests, and cumulative
  gates remain mandatory.

## Stop Conditions

Implementation must stop before writing if:

- A file has more than one owning family.
- A proposed directory is a generic bucket.
- A new family has no immediate consumer or test.
- Transaction rollback cannot be stated before boundary mutation.
- Generation ownership is ambiguous.

## Exit Criteria

- Family-tree gate rejects runtime-root file flattening.
- Open rollback leaves decoder and renderer closed.
- Pause/resume/stop command and renderer state stay consistent.
- Seek resets decoder, queue, renderer, generations, and session position.
- Stale source/decode blocks fail before queue mutation.
- Close and shutdown work from active and failed states.
- Deterministic tests, full build, CTest, all gates, and hardware regressions
  pass.

## Completion Evidence

- Runtime family roots contain no directly flattened source files.
- Largest runtime production file: 139 lines.
- Largest runtime test file: 152 lines.
- Runtime responsibility-overflow limit: 260 lines, enforced by genealogy gate.
- MSVC Debug build passes with `/W4 /WX`.
- CTest passes 11/11.
- Audio core gates pass 9/9.
- WAV, FLAC, and MP3 each deliver 96,000 decoded, queued, and accepted frames.
- Hardware smoke reports zero wait timeouts, underruns, and device invalidations.
