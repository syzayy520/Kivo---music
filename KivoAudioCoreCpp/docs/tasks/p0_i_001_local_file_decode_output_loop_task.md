# P0-I-001 Local File Decode-to-Output Loop

**Task:** KIVO-CPP-AUDIO-CORE-P0-I-001-LOCAL-FILE-OUTPUT
**Status:** COMPLETED
**Execution Basis:** `docs/architecture/audio_core_execution_roadmap.md`
**Last Updated:** 2026-06-12

## Purpose

Prove the first complete backend path from a real local file through the
quarantined FFmpeg adapter, bounded render transport, dedicated MMCSS render
worker, and real WASAPI shared output.

## Base Gate

- WASAPI shared event-driven renderer passes real hardware output.
- SPSC queue, RenderPump, and MMCSS worker pass deterministic tests.
- FFmpeg LGPL shared provenance is locked and externally supplied.
- WAV, FLAC, and MP3 decode adapter tests pass with explicit conversion.

## Natural Family Tree

```text
local file
  -> LocalFileByteSource
  -> FfmpegAvioBridge
  -> FfmpegDemuxSession
  -> FfmpegAudioDecoder
  -> FfmpegAudioConverter
  -> DecodedAudioBlockView
  -> producer lookahead and bounded block splitting
  -> SpscAudioBlockQueue
  -> RenderPump
  -> WasapiRenderWorker
  -> WasapiRenderer
  -> shared-mode endpoint
```

## Ownership and Thread Rules

- File I/O, demux, decode, conversion, and queue production run on the calling
  producer thread.
- Only `WasapiRenderWorker` calls the renderer write/wait path after start.
- Queue slots own every submitted render block.
- Decoder-owned bytes are copied before the next decoder call.
- One producer-side pending block is allowed solely to mark the true final
  render fragment as EOS.
- No FFmpeg call, allocation, file I/O, or logging occurs on the render thread.

## Generation Mapping

- Source generation maps to render stream generation.
- Decode generation maps to render buffer generation.
- Seek, flush, and device generations come from the active renderer snapshot.
- No generation is rewritten after a decoded block is produced.

## Required Evidence

- A generated real WAV, FLAC, and MP3 fixture can play through the hardware
  path.
- Renderer-accepted format is the decoder conversion target.
- Every decoded frame is queued exactly once.
- Worker accepted frames equal decoded and queued frames.
- Worker terminates as `EndOfStream`.
- Renderer drain succeeds after worker completion.
- MMCSS registration and high priority are true.
- Decode failure, queue failure, worker failure, drain failure, and device
  failure return distinct process exit paths.
- FFmpeg and Windows implementation types remain quarantined.

## Stop Conditions

- Decode is invoked on the render worker.
- A decoder view survives across the next decoder call without a copy.
- Queue pressure drops or duplicates frames.
- EOS is inferred from duration instead of decoder completion.
- A format mismatch is reinterpreted without conversion.
- Hardware success is claimed without accepted-frame equality.

## Exit Criteria

- The complete local-file path plays to completion on real hardware.
- All frame equality and worker state checks pass.
- Full cumulative CTest and repository gates pass.
