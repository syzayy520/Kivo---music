# P0-K-004 Lifecycle and Resource Stability

**Task:** KIVO-CPP-AUDIO-CORE-P0-K-004-LIFECYCLE-RESOURCE-STABILITY
**Status:** COMPLETED
**Execution Basis:** P0-K endurance and stress requirements
**Last Updated:** 2026-06-12

## Natural Family Tree

```text
tests/stability/
  fixture/
  platform/
    windows/
  runner/
  scenario/
```

## Required Evidence

- Two thousand deterministic open/start/write/consume/stop/close renderer cycles.
- Bounded observation history and reset counters after every close.
- Warmed multi-format FFmpeg decode lifecycle cycles.
- Two independent measured phases after warmup.
- Windows process handle count and PrivateUsage captured after each phase.
- Bounded handle and private-memory growth enforced by executable assertions.
- Actual resource values printed by the test executable.

## Gates

- Stability root contains zero direct files.
- Immediate subfamilies match the declared tree.
- Every stability file remains at or below 260 lines.
- The stability executable is registered with CTest when FFmpeg is configured.
- Full build, CTest, and cumulative repository gates pass.

## Completion Evidence

- Deterministic renderer lifecycle: 2,000 complete cycles passed.
- FFmpeg lifecycle: 6 warmup cycles plus two measured phases of 24 cycles each.
- Media rotation: generated PCM WAV, FLAC, and MP3 fixtures.
- Windows handle count: `168 -> 168 -> 168`.
- Process PrivateUsage: `29,478,912 -> 29,650,944 -> 29,671,424` bytes.
- Total positive private-memory growth: `192,512` bytes, below the 8 MiB limit.
- Second-phase positive private-memory growth: `20,480` bytes, below the 4 MiB limit.
- Handle growth: zero, below the two-handle limit.
- Stability family root files: zero.
- Largest stability source file: 102 lines, below the 260-line responsibility limit.
- Full FFmpeg-enabled build: passed.
- CTest: 12 of 12 test groups passed.
- Repository gate validation: all 9 gates passed.
