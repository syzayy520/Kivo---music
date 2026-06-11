# P0-F/H-004 WASAPI Render Worker

**Task:** KIVO-CPP-AUDIO-CORE-P0-FH-004-WASAPI-RENDER-WORKER
**Status:** COMPLETED
**Execution Basis:** `docs/architecture/audio_core_execution_roadmap.md`
**Depends On:** P0-F/H-001, P0-F/H-002, P0-F/H-003
**Last Updated:** 2026-06-12

## Purpose

Run the platform-neutral `RenderPump` on a dedicated Windows render thread,
wait on the WASAPI event outside `write`, and expose bounded atomic
observations to the control thread.

## Official API Basis

- MMCSS overview:
  <https://learn.microsoft.com/windows/win32/procthread/multimedia-class-scheduler-service>
- `AvSetMmThreadCharacteristicsW`:
  <https://learn.microsoft.com/windows/win32/api/avrt/nf-avrt-avsetmmthreadcharacteristicsw>
- `AvSetMmThreadPriority` and `AvRevertMmThreadCharacteristics` from
  `avrt.h`.

## Natural Family Tree

```text
include/kivo/platform/windows/wasapi/
  wasapi_render_worker_snapshot.hpp
  wasapi_render_worker.hpp

src/platform/windows/wasapi/
  wasapi_render_worker.cpp

tests/hardware/wasapi/
  wasapi_tone_smoke.cpp
```

## Ownership and Lifetime

- The renderer and queue are externally owned and must outlive the worker.
- `start` creates one worker thread only after the renderer is Started.
- `request_stop` is lock-free and nonblocking.
- `join` owns thread-handle closure.
- The destructor requests stop and joins indefinitely before releasing state.
- Renderer control operations remain on the COM-owning control thread.
- Only `write`, snapshot reads, and event wait execute on the render thread.

## Realtime Loop

1. Register the calling worker thread with MMCSS `Pro Audio`.
2. Request high relative MMCSS priority.
3. Execute `RenderPump::step`.
4. Continue immediately while progress is made.
5. Wait on the WASAPI event for idle or delayed results.
6. Stop on EOS, drained producer, explicit stop, rejection, or failure.
7. Revert MMCSS registration before thread exit.

## Diagnostics

- Worker state and last render failure.
- Last platform error and last WASAPI wait result.
- MMCSS registration and priority truth.
- Accepted frames and discarded stale blocks.
- Event wait timeout count.

## Exit Criteria

- Public headers contain no Windows types.
- Tone smoke uses producer queue plus dedicated render worker.
- Repeated hardware cycles accept every queued frame.
- MMCSS truth is printed and recorded.
- No allocation, logging, host callback, or FFmpeg call occurs in the loop.
- Full cumulative validation passes.
