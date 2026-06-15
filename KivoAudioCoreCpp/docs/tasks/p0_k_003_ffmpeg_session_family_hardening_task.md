# P0-K-003 FFmpeg Session Family Hardening

**Task:** KIVO-CPP-AUDIO-CORE-P0-K-003-FFMPEG-SESSION-FAMILY-HARDENING
**Status:** COMPLETED
**Execution Basis:** Genealogy iron rule and P0-K structural audit
**Last Updated:** 2026-06-12

## Finding

The FFmpeg decode-session implementation had reached 603 lines and combined
source ownership, demux opening, decoder priming, format negotiation, probe
construction, frame conversion, end-of-stream draining, seek, flush, close,
and exception-safe public forwarding. Its unit tests were also flat and one
scenario file had reached 263 lines.

## Natural Family Tree

```text
include/kivo/adapters/ffmpeg/
  session/

src/adapters/ffmpeg/
  codec/
  container/
  conversion/
  mapping/
  session/
  source/

tests/ffmpeg_decode/
  fixture/
  runner/
  scenario/
```

## Responsibility Rules

- Source owns the bounded AVIO bridge.
- Container owns demux lifecycle and packet selection.
- Codec owns decoder lifecycle and frame receipt.
- Conversion owns output-format conversion and drain.
- Mapping owns FFmpeg-to-core format and media vocabulary.
- Session open owns composition, priming, negotiation, and probe construction.
- Session step owns produced blocks, seek trimming, packet feed, and EOS drain.
- Session control owns seek, flush, close, generation reset, and probe query.
- Session facade owns exception containment and public boundary forwarding.
- Test fixture, runner, and independent scenario families remain separate.

## Gates

- Public, source, and unit-test roots contain zero direct files.
- Immediate subfamilies match the declared tree exactly.
- Every production and unit-test file remains at or below 260 lines.
- Old flat FFmpeg session and unit-test paths have zero references.
- Decode tests and FFmpeg/WASAPI integration targets build without behavior drift.
- Full build, CTest, and cumulative repository gates pass.

## Completion Evidence

- Public, source, and unit-test roots contain zero direct files.
- Old flat FFmpeg session and unit-test paths have zero references.
- Largest FFmpeg production file is 240 lines.
- The former 603-line session is split into open, step, control, facade, and runtime state.
- The former 263-line session test is split; its largest scenario is 98 lines.
- FFmpeg decode scenarios pass 7/7, including corrupt input and generation changes.
- FFmpeg decode and FFmpeg/WASAPI integration targets build.
- MSVC Debug full build passes with `/W4 /WX`.
- CTest passes 11/11 executables.
- Cumulative repository gates pass 9/9.
