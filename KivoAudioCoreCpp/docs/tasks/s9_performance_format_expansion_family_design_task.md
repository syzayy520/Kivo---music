# S9 Performance and Format Expansion

**Task:** KIVO-CPP-AUDIO-CORE-S9-PERFORMANCE-FORMAT-EXPANSION
**Status:** IMPLEMENTATION AUTHORIZED
**Execution Basis:** measured evidence, natural genealogy, single responsibility
**Last Updated:** 2026-06-13

## Objective

Close the repository-controlled portion of the Windows backend S9 program:

- prove WAV, FLAC, MP3, AAC, ALAC, Vorbis, and Opus through generated media;
- expose bounded queue occupancy history without adding blocking or allocation;
- capture repeatable open, first-block, seek, and full-decode latency samples;
- capture queue occupancy and consumer-interval jitter samples outside realtime;
- preserve explicit DSD-to-PCM, DoP, and native DSD truth from P0-Q;
- leave CPU/allocation profiling as a separately captured ETW/WPA evidence gate.

No performance claim is accepted without raw samples, environment identity, and
the exact executable and fixture revision that produced them.

## Current Audit

| Area | Existing evidence | Missing evidence |
|---|---|---|
| WAV/FLAC/MP3/AAC | generated fixtures and decode tests | none for the current baseline |
| Vorbis/Opus | FFmpeg codec mapping only | generated fixtures and decode proof |
| ALAC | MP4 container mapping only | codec contract, mapping, fixture, decode proof |
| queue occupancy | current used slots and queued frames | historical peak occupancy |
| latency | conversion algorithmic latency only | measured open/first-block/seek/decode distributions |
| jitter | no repository probe | bounded consumer-interval sample evidence |
| CPU/allocation | resource-growth stability tests | ETW/WPA profile capture and reviewed findings |

## Natural Family Tree

```text
include/kivo/core/decode/media/
  audio_codec.hpp

include/kivo/core/render/queue/
  spsc_audio_block_queue_snapshot.hpp
  spsc_audio_block_queue.hpp

src/core/render/queue/
  atomic/
    queue_atomic_counter.hpp
  spsc_audio_block_queue.cpp

tests/ffmpeg_decode/
  fixture/
    assertion/
      fixture_decode_assertion.hpp
      fixture_decode_assertion.cpp
    decode/
      ffmpeg_decode_test_factory.hpp
  scenario/
    decode/
      baseline_decode_tests.cpp
    format/
      extended_format_matrix_tests.cpp
  runner/
    ffmpeg_decode_test_cases.hpp
    ffmpeg_decode_tests_main.cpp

tests/render_queue/
  spsc_audio_block_queue_tests.cpp

tools/validation/performance/
  capture/
    capture_audio_core_performance_evidence.ps1
  foundation/
    latency_distribution.hpp
    performance_probe_model.hpp
  measurement/
    decode_latency_measurement.hpp
    decode_latency_measurement.cpp
    queue_timing_measurement.hpp
    queue_timing_measurement.cpp
  reporting/
    performance_evidence_writer.hpp
    performance_evidence_writer.cpp
  runner/
    audio_core_performance_probe_main.cpp
  verification/
    verify_audio_core_performance_evidence.ps1
```

`format/` is admitted because codec/container matrix behavior is distinct from
baseline conversion behavior. `performance/` is an offline validation family;
it is not linked into the shipping DLL and has no authority over runtime policy.

## Single-File Responsibilities

| File | Sole responsibility |
|---|---|
| `audio_codec.hpp` | enumerate codec identity exposed by the decode contract |
| `ffmpeg_audio_format.cpp` | translate FFmpeg identities into core identities |
| `fixture_decode_assertion.*` | execute and assert one generated fixture decode |
| `baseline_decode_tests.cpp` | register established baseline fixture cases |
| `extended_format_matrix_tests.cpp` | register ALAC/Vorbis/Opus fixture cases |
| `spsc_audio_block_queue_snapshot.hpp` | immutable queue observation value |
| `queue_atomic_counter.hpp` | saturating and monotonic atomic counter operations |
| `spsc_audio_block_queue.cpp` | bounded SPSC storage and lock-free counters |
| `latency_distribution.hpp` | derive integer nanosecond distribution values |
| `performance_probe_model.hpp` | define evidence records with explicit units |
| `decode_latency_measurement.*` | collect decode operation timing samples |
| `queue_timing_measurement.*` | collect occupancy and consumer-interval samples |
| `performance_evidence_writer.*` | serialize sanitized evidence JSON |
| `audio_core_performance_probe_main.cpp` | validate CLI and orchestrate probe steps |
| `capture_audio_core_performance_evidence.ps1` | bind measurements to hashes and environment provenance |
| `verify_audio_core_performance_evidence.ps1` | reject incomplete or dishonest evidence |

No generic `helper`, `common`, `misc`, `manager`, or `utils` family is allowed.
Every implementation or test file remains at or below 260 lines.

## Format Matrix Contract

All generated inputs are two-second synthetic sine waves and are build outputs,
never committed media. Each case must prove:

1. source opens through `LocalFileByteSource`;
2. codec and container identities are exact;
3. native sample rate and channel layout are exact;
4. requested output remains 48 kHz stereo interleaved float;
5. every produced block has contiguous frame offsets and exact byte extent;
6. decoded duration remains inside an explicit codec-appropriate bound;
7. close succeeds.

ALAC maps to `AudioCodec::Alac` and `MediaContainer::Mp4`. Vorbis and Opus map
to `MediaContainer::Ogg`. Adding a codec identity does not imply passthrough,
bit-perfect output, native DSD, or DoP support.

## Queue Observation Contract

The queue snapshot adds `observed_peak_used_slots`.

- It is monotonically nondecreasing between reset operations.
- It never exceeds `capacity_slots`.
- A successful push updates it from producer-observed occupancy before the
  producer cursor is published, preserving snapshot ordering.
- Failed, invalid, oversized, and closed pushes do not change it.
- `reset()` returns it to zero while producer and consumer are quiescent.
- Updates remain atomic, bounded, allocation-free, lock-free in the same sense
  as the existing atomic queue counters, and use no clock.

Queue timing jitter is sampled only by the offline performance probe. It is not
measured in `try_push`, `try_peek`, `consume_front`, or a WASAPI callback.

## Performance Evidence Contract

For each selected fixture and iteration, the probe records integer nanoseconds:

- `open_ns`;
- `first_block_ns`;
- `seek_ns`;
- `full_decode_ns`;
- decoded frame and block counts.

The report retains raw samples and derives minimum, median, p95, and maximum.
The queue probe records:

- capacity;
- sampled used-slot values;
- producer-observed peak used slots from the runtime snapshot;
- successful push and consume counts;
- consumer interval samples in nanoseconds;
- consumer interval p50, p95, and maximum.

Evidence must include UTC capture time, probe schema version, fixture names,
iteration count, and a sanitized environment description. Absolute local paths,
user names, media metadata, and audio payload bytes are forbidden.

The verifier checks schema, positive iteration count, expected fixture coverage,
nonnegative samples, percentile ordering, sample-count consistency, queue bounds,
and absence of obvious absolute Windows paths. It does not certify that a
machine meets an undeclared commercial latency target.

## CPU and Allocation Profile Gate

Timing is not a CPU or allocation profile. Commercial S9 completion additionally
requires a reviewed ETW/WPA capture covering:

- process CPU by stack during open, steady decode, seek, and render;
- allocation stacks outside and inside the realtime render path;
- hard faults and disk I/O during playback;
- DPC/ISR interference for physical-device runs;
- before/after evidence for every optimization.

The repository may provide capture and validation tooling, but the evidence must
come from a real signed release candidate on declared hardware.

## Validation

1. Configure and build the FFmpeg-enabled target from a clean build tree.
2. Generate and hash-lock all format fixtures.
3. Run FFmpeg decode tests, render queue tests, and all CTest groups.
4. Run the performance probe with at least five measured iterations after one
   warmup iteration.
5. Verify the generated performance evidence.
6. Run all repository gates.
7. Confirm every governed source, test, and validation script is at most 260
   lines and every family root contains only admitted subfamilies.

## Stop Conditions

Implementation must stop and report a blocker rather than weaken truth if:

- the pinned FFmpeg build lacks an encoder or decoder required by the matrix;
- a fixture cannot be generated deterministically from the pinned toolchain;
- a codec/container identity is ambiguous;
- queue observation requires a lock, allocation, clock, or cross-thread unsafe
  non-atomic state;
- performance evidence cannot retain raw samples;
- a threshold would be invented without a product requirement and baseline;
- CPU/allocation claims are requested without profiler evidence.

## Completion Truth

Repository completion means format tests, queue observation, performance capture
and verification tooling all pass. It does not mean commercial S9 is complete.
Commercial completion remains blocked until the signed release candidate has
reviewed ETW/WPA profiles and physical-device performance evidence.
