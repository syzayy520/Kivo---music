# P0-K-005 Hardware Smoke Family Refactor

**Task:** KIVO-CPP-AUDIO-CORE-P0-K-005-HARDWARE-SMOKE-FAMILY
**Status:** COMPLETED
**Execution Basis:** Strict genealogy and single-responsibility gate
**Last Updated:** 2026-06-12

## Reason

The original hardware smoke programs mixed command-line parsing, fixture
generation, device setup, queue production, worker lifecycle, diagnostics, and
process entry in files of 344 and 280 lines. Those files exceeded the runtime
family responsibility limit and obscured ownership boundaries.

## Natural Family Tree

```text
tests/hardware/
  decode_output/
    fixture/
    runner/
    scenario/
  wasapi/
    fixture/
    runner/
    scenario/
```

## Responsibility Boundaries

- `fixture`: deterministic options, formats, generated PCM, and owned context.
- `scenario`: one hardware behavior or lifecycle phase per source file.
- `runner`: argument contract and ordered scenario dispatch only.
- Hardware family roots contain zero direct files.
- Every C++ source or header in the hardware family remains at or below 260
  lines.

## Required Validation

- Both hardware smoke targets compile with warnings treated as errors.
- Existing command-line contracts and exit-code classes remain stable.
- Full CTest suite remains green.
- Genealogy gate explicitly owns both hardware subfamilies.
- Repository cumulative gates remain green.

## Completion Evidence

- Original 344-line WASAPI tone smoke removed.
- Original 280-line FFmpeg/WASAPI smoke removed.
- Hardware family root files: zero.
- Largest hardware family source: 150 lines.
- Both smoke targets compiled with MSVC `/W4 /WX`.
- Real WASAPI tone smoke: 4,800 submitted and accepted frames, zero
  underruns, zero wait timeouts, MMCSS registered at high priority.
- Real FFmpeg-to-WASAPI smoke: 96,000 decoded, queued, and accepted frames,
  zero underruns, zero device invalidations, MMCSS registered at high
  priority.
- Full FFmpeg-enabled build: passed.
- CTest: 12 of 12 test groups passed.
- Repository gate validation: all 9 gates passed.
