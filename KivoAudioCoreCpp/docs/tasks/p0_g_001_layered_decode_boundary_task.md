# P0-G-001 Layered Decode Boundary

**Task:** KIVO-CPP-AUDIO-CORE-P0-G-001-LAYERED-DECODE-BOUNDARY
**Status:** COMPLETED
**Owner Approval:** Satisfied by the active instruction to execute the roadmap
continuously through a usable backend
**Execution Basis:** `docs/architecture/audio_core_execution_roadmap.md`
**Last Updated:** 2026-06-12

## Purpose

Create the platform-neutral source and audio-decode boundary consumed by the
FFmpeg adapter without exposing any FFmpeg type, error code, allocation model,
or packet/frame lifetime.

## Base Gate

- Deterministic render boundary and fake backend pass.
- Bounded SPSC render queue and RenderPump pass.
- Real WASAPI worker path passes hardware output.
- FFmpeg LGPL shared development baseline is recorded with exact hash.
- Existing source, format, generation, queue, seek, and gapless contracts have
  been inventoried for reuse.

## Natural Family Tree

```text
include/kivo/core/decode/
  boundary/audio_decode_boundary.hpp
  block/decoded_audio_block_view.hpp
  failure/decode_failure.hpp
  generation/decode_generation.hpp
  media/audio_codec.hpp
  media/media_container.hpp
  media/media_probe.hpp
  request/decode_open_request.hpp
  result/decode_control_result.hpp
  result/decode_open_result.hpp
  result/decode_step_result.hpp

include/kivo/core/decode/source/
  byte_source_boundary.hpp
  byte_source_result.hpp

include/kivo/source/local/
  local_file_byte_source.hpp

src/source/local/
  local_file_byte_source.cpp

tests/decode_boundary/
  decode_boundary_tests_main.cpp
  decode_boundary_tests_main.hpp
  decode_contract_tests.cpp
  local_file_byte_source_tests.cpp
```

## Layered FFmpeg Family Reservation

```text
include/kivo/adapters/ffmpeg/
  ffmpeg_audio_decode_session.hpp

src/adapters/ffmpeg/
  source/ffmpeg_avio_bridge.*
  container/ffmpeg_demux_session.*
  codec/ffmpeg_audio_decoder.*
  conversion/ffmpeg_audio_converter.*
  mapping/ffmpeg_audio_format.*
  ffmpeg_audio_decode_session.cpp
```

## Boundary Rules

- `ByteSourceBoundary` owns source read/seek semantics.
- The decoder takes exclusive ownership of one byte source while open.
- `MediaProbe` reports native format, target format, duration, codec,
  container, conversion participation, seekability, delay, and padding.
- `DecodedAudioBlockView` references decoder-owned output valid until the next
  decode/control/close call.
- The caller must copy a produced block into its owned bounded queue before
  calling decode again.
- Source and decode generations accompany every produced block.
- EOS is a distinct successful terminal result.
- Source, demux, codec, conversion, seek, flush, and boundary failures remain
  distinguishable.
- Unsupported remote source, seek, or flush never reports success.

## Initial Supported Scope

- Local regular files.
- WAVE/PCM, FLAC, and MP3 decode paths when FFmpeg reports support.
- Mono or stereo source in supported integer/float formats.
- Explicit conversion into the renderer's accepted interleaved format through
  libswresample.
- No network, NAS, WebDAV, credentials, cover art, lyrics, video decode, or
  media library.

## Realtime and Thread Rules

- Decode and file I/O never execute on the WASAPI render thread.
- FFmpeg never receives a queue-owned render buffer.
- The adapter may allocate on the decode producer thread.
- The render queue and renderer remain allocation-free in steady state.
- No FFmpeg callback reaches the host or UI.

## Tests

- Contract validity and result invariant tests.
- Local source open/read/seek/size/error tests.
- FFmpeg token quarantine for every public header.
- Probe and decode of generated PCM WAVE.
- Probe and decode of generated FLAC.
- MP3 conversion path when fixture generation is available.
- Truncated/corrupt input failure classification.
- Explicit remote-source, seek, and flush behavior.
- Generation preservation on every decoded block.
- EOS without deadlock or duplicate frames.

## Stop Conditions

- Any `AV*` type or FFmpeg error code reaches a public header.
- GPL/nonfree build configuration is accepted.
- Dependency download occurs during configure, build, test, or gate execution.
- Format mismatch is silently reinterpreted.
- Decoder-owned bytes are retained after the next decode call.
- Source failure is reported as EOS.
- Old-generation decoded data is rewritten as current.

## Exit Criteria

- Platform-neutral boundary and local file source tests pass independently.
- FFmpeg adapter is enabled only with a validated external SDK root.
- Real local files decode into queue-compatible PCM.
- A real file plays to completion through SPSC queue and WASAPI worker.
- Full cumulative validation and 60-minute WASAPI hardware stability pass.
