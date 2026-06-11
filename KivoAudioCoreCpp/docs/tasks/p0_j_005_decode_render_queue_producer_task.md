# P0-J-005 Decode-to-Render Queue Producer

**Task:** KIVO-CPP-AUDIO-CORE-P0-J-005-DECODE-RENDER-QUEUE-PRODUCER
**Status:** COMPLETED
**Execution Basis:** `docs/architecture/audio_core_execution_roadmap.md`
**Last Updated:** 2026-06-12

## Purpose

Replace hardware-smoke glue with a reusable platform-neutral producer that
converts decoded blocks into bounded render-queue blocks.

## Natural Family Tree

```text
include/kivo/core/playback/pipeline/
  decode_render_queue_producer_configuration.hpp
  decode_render_queue_producer_result.hpp
  decode_render_queue_producer_snapshot.hpp
  decode_render_queue_producer.hpp

src/core/playback/pipeline/
  decode_render_queue_producer.cpp

tests/playback_pipeline/
  decode_render_queue_producer_tests.cpp
```

## Rules

- Allocate exactly two bounded decode buffers during creation.
- Do not allocate, sleep, or spin inside `step`.
- Hold one decoded block so only the true final block receives EOS.
- Split blocks to the configured queue-slot frame capacity.
- Preserve frame offsets and source/decode/render generations.
- Return explicit backpressure without losing pending data.
- Close the queue producer exactly once at EOS.

## Exit Criteria

- Lookahead EOS behavior is deterministic.
- Chunk offsets and final tail are exact.
- Queue-full backpressure preserves pending data.
- Oversized decoded blocks fail before queue mutation.
- WAV, FLAC, and MP3 real-device playback use this producer and preserve exact
  decoded, queued, and accepted frame counts.
- Full cumulative validation passes.
