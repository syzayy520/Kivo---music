# P0-F/H-002 Bounded SPSC Render Queue

**Task:** KIVO-CPP-AUDIO-CORE-P0-FH-002-BOUNDED-SPSC-RENDER-QUEUE
**Status:** COMPLETED
**Execution Basis:** `docs/architecture/audio_core_execution_roadmap.md`
**Last Updated:** 2026-06-12

## Purpose

Create the production transport between a non-realtime PCM producer and the
single realtime render consumer. The transport must be bounded, own copied PCM
bytes, preserve render metadata, and require no allocation, wait, lock, log,
exception, or callback during steady-state push/peek/consume operations.

## Natural Family Tree

```text
include/kivo/core/render/queue/
  spsc_audio_block_queue_configuration.hpp
  spsc_audio_block_queue_snapshot.hpp
  spsc_audio_block_queue.hpp

src/core/render/queue/
  spsc_audio_block_queue.cpp

tests/render_queue/
  render_queue_tests_main.cpp
  render_queue_tests_main.hpp
  spsc_audio_block_queue_tests.cpp
```

## Ownership Model

- The queue allocates all slot metadata and PCM storage during `create`.
- The producer retains ownership of its source view and copies one complete
  block into the next free queue slot.
- The queue owns the copied bytes until the consumer releases the full slot.
- `try_peek` returns a non-owning view into the current consumer slot.
- `consume_front` advances only the accepted prefix after a renderer write.
- A partial consume keeps the unwritten tail in the same queue-owned slot.
- The view returned by `try_peek` remains valid until the consumer mutates or
  releases the front slot.

## Concurrency Contract

- Exactly one producer may call `try_push`.
- Exactly one consumer may call `try_peek` and `consume_front`.
- Producer and consumer may run concurrently.
- Configuration, reset, and destruction require both sides to be quiescent.
- Producer publication uses release ordering.
- Consumer observation uses acquire ordering.
- Consumer release uses release ordering.
- Producer capacity observation uses acquire ordering.
- No mutex or condition variable is part of the queue.

## Preserved Metadata

Every queued block preserves:

- exact render format;
- frame count and frame offset;
- buffer identity and buffer generation;
- stream, seek, flush, and device generations;
- end-of-stream marker;
- exact frame-aligned PCM byte payload.

## Capacity Policy

- Slot count and maximum bytes per slot are fixed at creation.
- Invalid or unreasonably large configurations fail creation.
- A full queue rejects the newest push explicitly.
- Oversized blocks are rejected explicitly.
- The queue never grows and never drops an existing block implicitly.
- Snapshot counters are saturating where wrap would make diagnostics lie.

## Realtime Rules

The following operations are steady-state realtime safe:

- `try_push`;
- `try_peek`;
- `consume_front`;
- `snapshot`.

They must not:

- allocate or free memory;
- block or sleep;
- acquire a lock;
- perform I/O;
- format a message;
- throw;
- call the host;
- call FFmpeg;
- call a platform audio API.

## Required Tests

- Invalid configuration rejection.
- FIFO metadata and byte preservation.
- Explicit full and oversized rejection.
- Partial front consumption and exact tail view.
- EOS preservation through partial consumption.
- Ring wraparound.
- Close rejects new pushes while allowing queued data to drain.
- Snapshot occupancy and counters.
- Sustained two-thread producer/consumer ordering stress without sleeps.
- Public-header standalone compilation and all-header compilation.

## Stop Conditions

- A queue operation allocates in steady state.
- More than one producer or consumer is implied by the API.
- A partial write loses or duplicates frames.
- A stale generation is rewritten or normalized.
- Queue full silently overwrites existing audio.
- An `AudioBlockView` outlives the queue slot it references.
- Tests rely on wall-clock sleeps.

## Exit Criteria

- Queue tests pass under MSVC `/W4 /WX`.
- Cross-thread stress preserves every block and byte in order.
- Static gates authorize only the declared queue family and target.
- Full cumulative validation passes.
- The WASAPI render worker consumes this queue through the existing
  `RenderBoundary`.
