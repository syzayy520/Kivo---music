# P0-F/H-003 Render Pump

**Task:** KIVO-CPP-AUDIO-CORE-P0-FH-003-RENDER-PUMP
**Status:** COMPLETED
**Execution Basis:** `docs/architecture/audio_core_execution_roadmap.md`
**Depends On:** P0-F/H-002 bounded SPSC render queue
**Last Updated:** 2026-06-12

## Purpose

Define one deterministic, nonblocking render-consumer step between the bounded
SPSC queue and any `RenderWriteBoundary`. Device waiting and thread ownership
remain outside this class.

## Natural Family Tree

```text
include/kivo/core/render/pump/
  render_pump_result.hpp
  render_pump.hpp

src/core/render/pump/
  render_pump.cpp

tests/render_queue/
  render_pump_tests.cpp
```

## Step Rules

- Empty open queue returns `Idle`.
- Empty producer-closed queue returns `QueueDrained`.
- Full or partial renderer acceptance consumes exactly the accepted prefix.
- Completing an EOS block returns `EndOfStream`.
- Delayed writes leave the queue unchanged and preserve retry timing.
- Stale generation rejection discards that complete stale queue block.
- Other rejection leaves ownership in the queue for control-plane action.
- Renderer failure leaves ownership in the queue for recovery policy.
- Internal ownership mismatch is reported as boundary failure.

## Realtime Rules

`step` performs no allocation, wait, lock, log, exception, platform call, host
callback, or FFmpeg call.

## Exit Criteria

- Deterministic tests cover every disposition.
- Partial writes expose the exact next tail.
- Stale blocks cannot permanently wedge the queue.
- Non-stale failures never silently discard caller-owned audio.
- Full cumulative validation passes.
