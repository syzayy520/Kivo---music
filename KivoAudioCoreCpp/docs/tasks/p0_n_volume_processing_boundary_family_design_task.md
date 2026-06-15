# P0-N Volume and Processing Boundary

**Task:** KIVO-CPP-AUDIO-CORE-P0-N-PROCESSING-BOUNDARY
**Status:** IMPLEMENTATION AND VALIDATION COMPLETE
**Execution Basis:** P0-M closeout and execution roadmap
**Last Updated:** 2026-06-12

## Goal

Add explicit ReplayGain, software volume, conversion quality, channel-remix,
dither, latency, tail, and DSP ownership without weakening strict bypass or
bit-perfect truth.

## Base Gate

- P0-M clean build, CTest, repository gates, and hardware truth checks pass.
- The output-truth evaluator already rejects every processing and conversion
  fact that is active or unknown.
- The decode-to-render producer already owns two preallocated mutable copies
  before publication to the bounded render queue.
- FFmpeg `SwrContext` is the existing and only resample/remix/format converter.

## Inventory Gate

- No executable ReplayGain or software-volume stage exists.
- Decoder output views and render views are immutable.
- The producer copies decoder output before queue publication, which is the
  safe non-realtime mutation point.
- FFmpeg conversion is active but exposes only coarse conversion and resample
  facts; quality, dither, latency, and pending tail are not visible.
- `ProcessingParticipation` already owns stable ReplayGain, volume, DSP, and
  channel-remix vocabulary and must not be duplicated.

## ODR Gate

- Reuse `ConversionPolicy`, `ResampleDecision`, `ProcessingParticipation`,
  `RenderFormat`, and the existing decoder/producer contracts.
- Add no second resampler, queue, audio-block type, output-truth report, or
  sample-format enum.
- Core processing must not include FFmpeg, Windows, WASAPI, host, or UI types.
- FFmpeg conversion options and delay queries remain inside the adapter.

## Natural Family Tree

```text
include/kivo/core/processing/
  chain/
  configuration/
  policy/
  result/
  snapshot/

src/core/processing/
  chain/
  gain/
  planning/

tests/audio_processing/
  fixture/
  runner/
  scenario/
    bypass/
    formats/
    gain/
    policy/
```

Integration remains with existing owners:

```text
include/kivo/core/decode/
src/adapters/ffmpeg/conversion/
  converter/
  policy/
  truth/
src/core/playback/pipeline/
tests/ffmpeg_decode/
tests/playback_pipeline/
```

## Responsibility Gate

- Configuration owns requested policy and user values only.
- Planning validates policy, resolves effective gain, and declares active
  participation without touching samples.
- Gain owns format-specific sample mutation only.
- Chain owns ordered execution, deterministic dither state, and counters.
- Conversion snapshot owns resample/remix/encoding/dither/latency/tail facts.
- FFmpeg converter owns libswresample configuration and live conversion facts.
- Producer owns the single call site between copied decode bytes and queue
  publication.
- No implementation or test file may exceed 260 lines.

## Locked Processing Order

```text
FFmpeg format conversion / channel remix / resample
  -> ReplayGain
  -> software volume
  -> bounded DSP stage boundary
  -> integer quantization dither when requested
  -> render queue
```

ReplayGain and software volume are combined into one scalar only after both
individual participation facts are resolved. Combining arithmetic must not
combine ownership or diagnostics.

## Strict Bypass

- Default configuration performs no sample writes.
- Disabled stages and unity gain preserve every byte.
- Bit-perfect-required configuration rejects ReplayGain, non-unity volume,
  DSP, conversion dither, channel remix, and resampling.
- Dither runs only for an active integer quantization path.
- Unknown or unsupported policy is rejected rather than silently ignored.

## Gain Rules

- ReplayGain uses decibels and optional peak-aware clipping prevention.
- Software volume uses a finite linear scalar from silence through unity.
- Integer formats saturate to their declared valid-bit range.
- Int24 preserves the 24-in-32 left-aligned representation.
- Float formats use finite saturated output and report clipped samples.
- Dither is deterministic under a supplied seed for repeatable tests.

## Conversion Rules

- `SwrContext` remains the only resampler/remixer.
- Fast, balanced, and high quality policies map to explicit swresample
  parameters before initialization.
- TPDF dither is enabled only when the target is integer and precision is
  reduced.
- Converter snapshots expose actual active stages, quality, latency frames,
  and pending tail frames.
- Draining publishes remaining converter output before EOS.

## Tests

- Default and unity bypass are byte-identical for every supported format.
- ReplayGain and software volume have independent participation truth.
- Peak prevention limits effective gain.
- Silence, positive, negative, min, max, and clipping samples are covered.
- Int16, Int24-in-32, Int32, Float32, and Float64 are covered.
- Deterministic TPDF has repeatable output and never runs in bypass.
- Bit-perfect-required mode rejects every mutation request.
- Producer mutation occurs before queue publication and snapshots propagate.
- FFmpeg conversion policy reports resample/remix/encoding/dither and delay.
- Existing decode, pipeline, gapless, runtime, output-truth, and hardware paths
  remain green.

## STOP Conditions

- A second resampler or queue is introduced.
- Processing mutates decoder-owned or queue-owned immutable memory.
- Unity/default bypass writes samples.
- Bit-perfect-required mode silently disables requested processing.
- ReplayGain, volume, DSP, remix, resample, or dither participation is hidden.
- Adapter details enter a public core header.
- A processing implementation file exceeds 260 lines.

## Readiness Verdict

```text
KIVO-AUDIO-CORE-P0-N-FAMILY-DESIGN-LOCKED-001
IMPLEMENTATION_AUTHORIZED_BY_ACTIVE_EXECUTION_ROADMAP
P0_N_EXIT_CRITERIA_PASSED
```
