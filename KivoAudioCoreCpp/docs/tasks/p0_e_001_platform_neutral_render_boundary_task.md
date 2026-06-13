# P0-E-001 Platform-Neutral Render Boundary

**Task:** KIVO-CPP-AUDIO-CORE-P0-E-001-PLATFORM-NEUTRAL-RENDER-BOUNDARY  
**Status:** COMPLETE  
**Execution Basis:** `docs/architecture/audio_core_execution_roadmap.md`  
**Last Updated:** 2026-06-12

## Purpose

Define the smallest production-quality audio render boundary that can be
implemented unchanged by both the deterministic fake renderer and the WASAPI
adapter.

The boundary must make lifecycle, format acceptance, generation validity,
partial writes, ownership, delay, rejection, and failure observable without
leaking platform types or requiring synchronous realtime logging.

## Base Gate

- P0-D-018 is complete and committed.
- Clean MSVC/Ninja build passes.
- Contract tests pass.
- Capability tests pass.
- Public headers compile independently and together.
- Repository gates pass.

## Inventory Gate

The boundary reuses these existing contract families:

- `AudioFormatDescriptor` and `RenderFormat`.
- `FrameCount`, `SamplePosition`, and `BufferId`.
- `BufferGeneration`.
- `StreamGeneration`, `SeekGenerationId`, `FlushGeneration`, and
  `DeviceGeneration`.
- `RenderCapability`.

No competing identity, format, frame-count, or generation primitive is
introduced.

## ODR Gate

- Every new public type has a unique name.
- Public headers compile independently.
- All public headers compile in one translation unit.
- No type is re-declared in the fake or platform adapter families.

## Natural Family Tree

```text
include/kivo/core/render/
  identity/
    renderer_identity.hpp
  capability/
    renderer_capability_snapshot.hpp
  generation/
    render_generation_set.hpp
  failure/
    render_failure.hpp
  format/
    render_open_request.hpp
    render_open_result.hpp
  lifecycle/
    render_lifecycle_state.hpp
    render_control_request.hpp
    render_control_result.hpp
  buffer/
    audio_block_view.hpp
  write/
    render_write_result.hpp
  snapshot/
    render_snapshot.hpp
  boundary/
    render_control_boundary.hpp
    render_write_boundary.hpp
    render_boundary.hpp

tests/render_boundary/
  render_boundary_tests_main.cpp
  render_boundary_tests_main.hpp
  identity_capability_tests.cpp
  format_lifecycle_tests.cpp
  audio_block_view_tests.cpp
  render_write_result_tests.cpp
  boundary_interface_tests.cpp
```

The tree follows ownership and behavior families. No generic `common`,
`helper`, `utils`, `types`, or `manager` directory is allowed.

## Boundary Split

`RenderControlBoundary` is non-realtime. It owns identity and capability
queries plus open, start, flush, drain, reset, stop, close, and snapshot.

`RenderWriteBoundary` is realtime-facing. Its only operation is a
non-throwing write of an immutable `AudioBlockView`.

`RenderBoundary` combines both interfaces. The fake renderer and WASAPI
adapter must implement this combined boundary without changing core types.

## Data and Ownership Rules

- `AudioBlockView` is non-owning and immutable.
- The caller guarantees that bytes remain valid for the duration of `write`.
- Accepted frames transfer to the renderer.
- Any unwritten frames remain owned by the caller.
- Full writes leave no unwritten ownership.
- Partial writes report exact accepted and remaining frame counts.
- Delayed, rejected, and failed writes accept zero frames.
- A retry view is derived without allocation by advancing byte and stream
  offsets over the accepted prefix.

## Lifecycle Rules

```text
Closed -> Open -> Started -> Draining
   ^        |        |          |
   |        v        v          v
   +------ Stopped <-+-------- Stopped

Any operational state may enter Failed.
Close returns the boundary to Closed.
Reset preserves an open renderer but clears queued render state.
Flush invalidates older flush/seek generations.
```

Concrete implementations enforce legal transitions. The boundary represents
invalid-state rejection explicitly and never relies on exceptions.

## Failure Matrix

| Condition | Write disposition | Frames accepted | Failure |
| --- | --- | ---: | --- |
| Entire block accepted | Full | all | None |
| Prefix accepted | Partial | prefix | None |
| Capacity temporarily unavailable | Delayed | 0 | None |
| Stream generation stale | Rejected | 0 | StaleStreamGeneration |
| Seek generation stale | Rejected | 0 | StaleSeekGeneration |
| Flush generation stale | Rejected | 0 | StaleFlushGeneration |
| Device generation stale | Rejected | 0 | StaleDeviceGeneration |
| Invalid block shape | Rejected | 0 | InvalidBuffer |
| Format invalid | Rejected | 0 | InvalidFormat |
| Format valid but unavailable | Rejected | 0 | UnsupportedFormat |
| Write in illegal lifecycle state | Rejected | 0 | InvalidState |
| Device invalidated | Failed | 0 | DeviceLost |
| Backend boundary failure | Failed | 0 | BoundaryFailure |

Underrun, overrun, and timeout are first-class failure values even when a
specific implementation reports them through snapshot or control results.

## Tests

- Identity and capability validity.
- Exact and compatible format request policy.
- Accepted and rejected open results.
- Lifecycle control result consistency.
- Audio block byte/frame validation and overflow safety.
- Zero-allocation retry view after a partial write.
- Full, partial, delayed, rejected, and failed write invariants.
- Compile-time interface shape and non-throwing operations.
- Public-header standalone and aggregate composition.

## Gate Changes

- Authorize only `include/kivo/core/render/` and `tests/render_boundary/`.
- Authorize `kivo_core_render`, `kivo_render_boundary_tests`, and
  `kivo_public_header_checks`.
- Continue rejecting platform APIs, FFmpeg APIs, runtime source trees, and
  third-party dependencies.
- Keep `tests/render/` blocked because it is ambiguous with the historical
  video-render scope.

## Stop Conditions

Implementation must stop and be repaired before continuing if:

- A Windows, WASAPI, COM, FFmpeg, or host type reaches the boundary.
- A write result leaves unwritten ownership ambiguous.
- A public header fails standalone or aggregate compilation.
- A realtime-facing operation can throw.
- Tests require sleep timing.
- The fake backend would need a private competing API.

## Exit Criteria

- All planned public types and interfaces are implemented.
- Tests prove value invariants and interface shape.
- The full validation batch passes.
- The task is committed and pushed.
- The deterministic fake renderer can begin directly against this boundary.

## Validation Record

Final local closeout on 2026-06-12:

- `PASS_BUILD_VALIDATION_NINJA`
- 229 public headers compiled independently.
- One aggregate translation unit compiled all 229 public headers together.
- Contract tests: 892/892 passed.
- Capability tests: 68/68 passed.
- Render boundary tests: 22/22 passed.
- CTest: 3/3 targets passed.
- Repository gates: 9/9 passed.
- Platform/codec token audit passed.
- `git diff --check` passed.
