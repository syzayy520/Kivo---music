# P0-L Gapless Timeline Accuracy

**Task:** KIVO-CPP-AUDIO-CORE-P0-L-GAPLESS-TIMELINE-ACCURACY
**Status:** COMPLETED
**Execution Basis:** P0-K implementation closeout and execution roadmap
**Last Updated:** 2026-06-12

## Goal

Provide sample-domain, gapless track transitions whose queue contents,
timeline positions, format compatibility, and whole-file segment boundaries
are explicit and testable.

## Base Gate

- P0-K implementation and repository gates pass.
- The FFmpeg adapter already reports decoder delay, trailing padding, and
  audible output duration.
- The decode-to-render producer already owns lookahead, chunking, generation
  rejection, and bounded queue transfer.
- The SPSC queue remains the only realtime transport. Gapless work does not
  add a second audio queue.

## Inventory Gate

- `MediaProbe::codec_delay` and `MediaProbe::trailing_padding` are source
  metadata. FFmpeg owns decoder-side trimming; the playback producer must not
  trim those samples a second time.
- `MediaProbe::duration_frames` is the adapter-resolved audible frame count in
  output-frame units.
- Existing CUE contracts are value vocabulary only. P0-L adds validation and
  planning behavior without duplicating their public types.
- Existing track-local producer offsets must be mapped into one monotonic
  timeline before queue publication.

## ODR Gate

- Reuse `RenderFormat`, `GaplessMetadata`, `VirtualTrackSegment`,
  `TrackSegmentRange`, `SamplePosition`, and `FrameCount`.
- Do not introduce a second PCM format, sample-position alias, queue, decoder
  boundary, or CUE range model.
- Gapless runtime behavior belongs below `core/playback/gapless`; it must not
  depend on FFmpeg, WASAPI, Windows, COM, or host ABI headers.

## Natural Family Tree

```text
include/kivo/core/playback/gapless/
  coordinator/
  plan/
  position/
  segment/
  transition/

src/core/playback/gapless/
  coordinator/
  position/
  segment/
  transition/

tests/playback_gapless/
  fixture/
  runner/
  scenario/
    compatibility/
    continuity/
    position/
    segment/
```

Existing producer extensions remain in their owning Pipeline families:

```text
include/kivo/core/playback/pipeline/
  configuration/
  producer/
src/core/playback/pipeline/
  step/
  transfer/
```

## Responsibility Gate

- Plan owns immutable track facts and safe timeline bounds.
- Transition owns compatibility classification only.
- Segment owns whole-file source-to-timeline mapping only.
- Position owns decoded, queued, rendered, and estimated-audible frontier
  semantics only.
- Coordinator owns two prepared producers and track advancement only.
- Pipeline configuration owns timeline origin and queue terminal policy.
- Pipeline transfer owns offset translation and final EOS publication.
- No file may exceed 260 lines.

## Timeline Proof

- Positions are frame frontiers: a value identifies the first frame not yet in
  that stage.
- For a valid snapshot:
  `estimated_audible <= rendered <= queued <= decoded`.
- A compatible next track starts exactly at the current track timeline end and
  has an identical render format.
- A non-final producer never closes the queue and never publishes an EOS flag.
- The final producer alone publishes EOS and closes the producer side.
- Track-local offsets are checked for overflow before adding timeline origin.

## Whole-File Segment Proof

- Segment indices are ordered and unique.
- Source ranges are non-empty, adjacent, start at zero, and cover the declared
  whole-file frame count exactly.
- Timeline ranges are contiguous and preserve every segment length.
- Insufficient output capacity or invalid ranges fail before partial output is
  reported as success.

## Verification Matrix

- Compatible two-track PCM fixtures preserve byte order and exact offsets.
- The first track publishes no EOS; the final track publishes exactly one EOS.
- Format mismatch, timeline gap, and timeline overlap have distinct reasons.
- Whole-file segments map exactly and reject malformed coverage.
- Position snapshots reject impossible ordering and estimate audible frontier
  from device-buffered frames.
- Existing Pipeline, Runtime, FFmpeg, WASAPI, stability, and contract tests
  remain green.

## STOP Conditions

- FFmpeg metadata would be trimmed again in playback.
- A platform header enters the gapless core.
- A transition can close the shared queue before the final track.
- Timeline arithmetic can wrap.
- A new generic helper/common/manager family is required.
- Tests prove only counts while ignoring offsets, EOS flags, or PCM bytes.

## Readiness Verdict

```text
KIVO-AUDIO-CORE-P0-L-FAMILY-DESIGN-LOCKED-001
IMPLEMENTATION_COMPLETE
```

## Completion Evidence

- `MediaProbe` duration, decoder delay, and trailing padding map into one
  immutable track plan without playback-side re-trimming.
- The rolling coordinator prefetches one successor into bounded private
  producer storage before the current track completes.
- Non-final producers preserve the shared queue and suppress EOS publication.
- The final producer alone publishes EOS and closes the queue.
- Compatible track PCM bytes remain ordered at exact offsets `0` and `4`;
  no silence block or dropped frame is introduced.
- Format mismatch, timeline gap, timeline overlap, final-track successor,
  occupied prefetch slot, missing successor, and prefetch failure are explicit.
- Whole-file segments preserve exact source coverage and contiguous timeline
  ranges.
- Position semantics expose decoded, queued, rendered, and estimated-audible
  frame frontiers.
- P0-L scenarios pass 11 of 11; Pipeline scenarios pass 9 of 9.
- Full FFmpeg-enabled MSVC Debug build passes with `/W4 /WX`.
- CTest passes 13 of 13 executable groups.
- Repository gates pass 9 of 9.
- Largest P0-L implementation file is 102 lines.
