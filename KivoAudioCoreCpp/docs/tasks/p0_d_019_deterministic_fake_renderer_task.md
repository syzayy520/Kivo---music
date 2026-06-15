# P0-D-019 Deterministic Fake Renderer

**Task:** KIVO-CPP-AUDIO-CORE-P0-D-019-DETERMINISTIC-FAKE-RENDERER  
**Status:** COMPLETE  
**Execution Basis:** `docs/architecture/audio_core_execution_roadmap.md`  
**Boundary Basis:** `docs/tasks/p0_e_001_platform_neutral_render_boundary_task.md`  
**Last Updated:** 2026-06-12

## Purpose

Implement a deterministic renderer that uses the exact platform-neutral
boundary intended for WASAPI and proves failure behavior without wall-clock
sleeps, platform APIs, hidden queues, or private competing interfaces.

## Base Gate

- P0-E-001 is complete, committed, pushed, and batch-validated.
- `RenderBoundary` splits non-realtime control from realtime-facing writes.
- Write ownership and generation semantics are explicit.

## Natural Family Tree

```text
include/kivo/testing/
  time/
    manual_clock.hpp
  render/
    configuration/
      fake_renderer_configuration.hpp
    fault/
      scheduled_render_fault.hpp
    observation/
      render_observation.hpp
      bounded_observation_history.hpp
    fake_renderer.hpp

src/testing/
  time/
    manual_clock.cpp
  render/
    fake_renderer.cpp

tests/fake_renderer/
  fake_renderer_tests_main.cpp
  fake_renderer_tests_main.hpp
  fake_renderer_test_fixture.hpp
  manual_clock_tests.cpp
  lifecycle_tests.cpp
  write_and_generation_tests.cpp
  fault_injection_tests.cpp
  observation_history_tests.cpp
```

This is explicit test infrastructure, not production device code. No generic
helper, utility, manager, or miscellaneous family is introduced.

## Determinism Rules

- Time advances only through `ManualClock::advance` or deterministic frame
  consumption/drain calculations.
- Tests never sleep to wait for behavior.
- Render capacity and maximum write size are configured.
- Faults trigger by exact write count, frame position, stream generation, next
  consume, or next drain.
- Fault slots and observation slots are fixed-capacity.
- Realtime-facing `write` performs no allocation and no synchronous logging.
- Observation order is stable and oldest entries are overwritten only when
  the configured history capacity is reached.
- The first implementation uses a serialized deterministic caller model.
  Deterministic concurrent interleaving belongs to the second fault set;
  unsynchronized concurrent calls are not claimed safe by this milestone.

## First Fault Matrix

| Required fault | Trigger | Expected proof |
| --- | --- | --- |
| Stale generation | Generation comparison | Rejected, zero frames accepted |
| Partial write | Scheduled write fault | Exact accepted prefix and caller-owned tail |
| Forced underrun | Next consume | Buffered frames consumed, underrun observed |
| Device lost during write | Scheduled write fault | Failed write and renderer state Failed |
| Invalid/unsupported format | Open validation | Rejected with explicit format failure |
| Drain timeout | Next drain or insufficient timeout | TimedOut, no sleep, state Draining |

## Lifecycle Model

- Constructed state is Closed.
- Open validates request, device generation, format, and configured capacity.
- Start transitions Open or Stopped to Started.
- Write is accepted only in Started.
- Flush clears buffered frames and adopts the supplied generations.
- Drain consumes buffered frames only when the deterministic timeout budget is
  sufficient.
- Reset clears buffered state and returns to Open.
- Stop returns to Stopped.
- Close clears accepted format and returns to Closed from any state.
- Device loss transitions to Failed and remains observable until close/reset.

## Observation Contract

Every lifecycle operation, write, consume, fault scheduling decision, and
failure records:

- Operation kind.
- Monotonic manual-clock timestamp.
- Lifecycle state after the operation.
- Failure, if any.
- Submitted/accepted frame counts.
- Write or consume ordinal.
- Active generation set.

History reads are control-side operations. Recording is bounded and
allocation-free.

## Static Structure Checks

- Fake backend implements `RenderBoundary` directly.
- No Windows, COM, WASAPI, FFmpeg, host, or UI type.
- No sleep or wall-clock dependency.
- No dynamic allocation in `write`, consume, or observation recording.
- Public headers compile independently and together.
- Fault and history capacities are compile-time bounded.

## Tests

- Manual clock monotonic and saturating behavior.
- Open/start/stop/reset/close lifecycle.
- Exact format and unsupported format.
- Normal full write and natural capacity partial write.
- All four generation mismatches.
- Scheduled partial write by count and frame position.
- Device lost during write.
- Forced underrun during consume.
- Drain timeout and successful deterministic drain.
- Bounded history overwrite order.
- Fixed fault table exhaustion and slot reuse.

## Gate Changes

- Authorize `include/kivo/testing/`, `src/testing/`, and
  `tests/fake_renderer/` only for the deterministic backend.
- Authorize `kivo_fake_render_backend` and `kivo_fake_renderer_tests`.
- Continue rejecting platform, codec, host, third-party, demo, and sample
  targets.

## Stop Conditions

- The fake requires a different public boundary than WASAPI.
- Any mandatory fault depends on sleep timing.
- Write or observation recording allocates.
- A stale generation accepts frames.
- Partial-write ownership becomes ambiguous.
- History can grow without bound.
- Full validation regresses existing contract or boundary tests.

## Exit Criteria

- The first mandatory fault set is deterministic and repeatable.
- The fake implements `RenderBoundary`.
- All fake tests and cumulative tests pass.
- Public-header composition and repository gates pass.
- The task is committed, pushed, and batch-validated.

## Validation Record

Final local closeout on 2026-06-12:

- `PASS_BUILD_VALIDATION_NINJA`
- 235 public headers compiled independently.
- One aggregate translation unit compiled all 235 public headers together.
- Contract tests: 892/892 passed.
- Capability tests: 68/68 passed.
- Render boundary tests: 22/22 passed.
- Fake renderer tests: 24/24 passed.
- CTest: 4/4 targets passed.
- Repository gates: 9/9 passed.
- Determinism gate rejected sleep APIs, dynamic sequence containers, and
  explicit heap allocation APIs in the fake backend.
- Platform/codec token audit passed.
- `git diff --check` passed.
