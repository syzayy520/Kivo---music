# P0-N Processing Boundary Closeout

**Status:** IMPLEMENTATION COMPLETE
**Date:** 2026-06-12

## Delivered

- Byte-identical default and unity bypass.
- ReplayGain with optional peak clipping prevention.
- Software volume with independent participation truth.
- Int16, Int24-in-32, Int32, Float32, and Float64 gain paths.
- Deterministic TPDF dither for active integer processing.
- Processing before immutable render-queue publication.
- Explicit FFmpeg fast, balanced, and high resample policies.
- Conversion dither only for integer precision reduction.
- Conversion latency and pending-tail snapshots.
- Complete bit-perfect evidence assembly from conversion and processing facts.
- Playback-runtime policy propagation and strict bit-perfect conflict rejection.

## Family Gate

Core processing is grouped under configuration, planning, gain, chain, result,
policy, and snapshot owners. FFmpeg conversion is split into converter, policy,
and truth subfamilies. Playback-pipeline scenarios are grouped by behavior.
The largest related implementation or test file is 130 lines.

## Verification

- Full FFmpeg-enabled MSVC Debug build: 632 compile/link steps completed.
- CTest: 15 of 15 passed.
- Audio processing: 9 of 9 passed.
- Output truth: 8 of 8 passed.
- Playback pipeline: 10 of 10 passed.
- Playback runtime: 15 of 15 passed.
- FFmpeg decode: 18 of 18 passed.
- Repository gates: 9 of 9 passed.
- Public-header standalone and aggregate compilation: passed.

## Hardware Truth

The active integrated speaker endpoint completed an FFmpeg-to-WASAPI shared
playback of a 44.1 kHz FLAC converted to 48 kHz. All 96000 decoded frames were
queued and accepted. MMCSS registration and high priority were active, with
zero wait timeouts, underruns, or device invalidations.

## Next Phase

P0-O owns the versioned C ABI, opaque handles, extensible structures,
capability negotiation, callback and ownership rules, host-serialized
commands, sanitized diagnostics, and Tauri-facing integration boundary.
