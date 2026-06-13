# P0-Q Hi-Res, DSD, and DoP Research

**Task:** KIVO-CPP-AUDIO-CORE-P0-Q-HIRES-DSD-DOP-RESEARCH
**Status:** CONTRACTS AND EXPLICIT RUNTIME REJECTION COMPLETE; PLAYBACK RESEARCH ONLY
**Execution Basis:** P0-P engineering RC closeout and execution roadmap
**Last Updated:** 2026-06-12

## Goal

Research Hi-Res PCM, native DSD, and DSD-over-PCM (DoP) support without
changing the current PCM playback claims. P0-Q decides the future contract,
decode, render, truth, validation, licensing, and device-evidence requirements
before any runtime support is implemented.

## Base Gate

- P0-P engineering release-candidate evidence remains intact.
- Current PCM, exclusive, bit-perfect, processing, ABI, and release claims do
  not change.
- Existing PCM `AudioFormatDescriptor` semantics remain truthful.
- DSD and DoP are not represented as ordinary PCM unless an explicit DoP
  carrier contract says so.

## Inventory Gate

- Hi-Res PCM can reuse the existing sample-rate, valid-bit, container-bit, and
  channel-layout model if validation limits are extended deliberately.
- Native DSD is not PCM and must not be hidden behind `SampleFormat::Int16`,
  `SampleFormat::Int24`, `SampleFormat::Int32`, `Float32`, or `Float64`.
- DoP is a PCM transport carrier for DSD payload markers, not PCM audio
  content to be processed by volume, ReplayGain, resampling, remix, or dither.
- FFmpeg may decode some DSD containers to PCM depending on build support; that
  path is conversion, not native DSD or DoP passthrough.
- Windows endpoint support for native DSD or DoP must be runtime-probed and
  stored as device evidence before any user-facing claim.

## Research Evidence

Primary references captured for this phase:

- DoP Open Standard v1.1: `https://dsd-guide.com/sites/default/files/white-papers/DoP_openStandard_1v1.pdf`
- FFmpeg formats and codec documentation: `https://ffmpeg.org/ffmpeg-formats.html`,
  `https://ffmpeg.org/ffmpeg-codecs.html`
- Microsoft audio endpoint and WASAPI documentation:
  `https://learn.microsoft.com/en-us/windows/win32/coreaudio/wasapi`

These references are research inputs only. Repository behavior must be based on
local tests, exact dependency versions, and device evidence.

## Natural Family Tree

Research ownership:

```text
docs/tasks/
  p0_q_hi_res_dsd_dop_research_family_design_task.md
docs/validation/
  p0_q_hi_res_dsd_dop_research_2026_06_12.md
```

Future implementation, if approved, must grow under existing families first:

```text
include/kivo/core/contract/format/
  descriptor/       # PCM truth extensions only
  dsd/              # native DSD and DoP carrier contracts

include/kivo/core/output/truth/
  evidence/         # native DSD, DoP, conversion, and processing facts
  evaluation/       # claim decision priority
  result/

src/adapters/ffmpeg/
  container/        # DSF, DSDIFF, SACD-derived container facts
  codec/            # decoded PCM path facts
  conversion/       # DSD-to-PCM conversion evidence only

src/platform/windows/wasapi/
  format/           # endpoint format probing and carrier mapping
  renderer/         # mode-specific open and submit behavior

tests/
  hires_pcm/
  dsd_dop/
```

No top-level `dsd`, `hires`, `dop`, `sacd`, `formats`, or `compat` directory is
allowed.

## Responsibility Gate

- PCM descriptor owns PCM only.
- DSD contracts own native DSD rate, channel count, frame grouping, and packet
  identity only.
- DoP carrier contracts own marker cadence, PCM carrier rate, payload rate, and
  validation only.
- Output truth owns whether the actual path is Hi-Res PCM, converted PCM,
  native DSD, DoP, rejected, or unknown.
- FFmpeg adapter owns library-specific demux, decode, and conversion facts.
- WASAPI adapter owns endpoint probing, format mapping, and platform errors.
- Processing chain must reject native DSD and DoP passthrough unless an
  explicit conversion stage changes them into PCM with visible evidence.
- Host ABI may expose only stable capability and result facts after the core
  contract is proven.

## Implementation Admission Rules

P0-Q implementation may proceed only as one narrow slice at a time:

1. Hi-Res PCM validation and device proof.
2. DSD container probe with explicit unsupported playback.
3. DSD-to-PCM conversion with truth evidence.
4. DoP carrier validation and endpoint proof.
5. Native DSD passthrough proof on verified hardware.

Each slice must include tests, device evidence where applicable, and claim
review. A slice must not combine unrelated families just because the media
marketing label is the same.

## Completed Local Slice

Hi-Res PCM contract-level validation is present in:

```text
tests/hires_pcm/
  fixture/
  runner/
  scenario/
    boundary/
    descriptor/
```

This slice proves that high sample-rate PCM descriptors preserve sample rate,
valid bits, container bits, channel layout, and frame-size truth. It does not
claim device support or commercial Hi-Res playback.

Native DSD and DoP value-only contracts are present in:

```text
include/kivo/core/contract/format/dsd/
tests/contracts/format/dsd/
```

This slice gives DSD/DoP independent contract identities and carrier validation
facts. It does not claim decode support, render support, endpoint support,
SACD support, passthrough support, or commercial DSD/DoP playback.

Decode/media support classification is present in:

```text
include/kivo/core/decode/media/support/
tests/decode_boundary/scenario/media/
```

This slice lets the decode boundary identify DSD containers and codec ids,
then reject native DSD before opening the PCM decoder path. DoP carriers remain
an explicit unsupported policy input. It does not convert samples, render DSD,
render DoP, or change product claims.

## STOP Conditions

- DSD is added as a normal PCM sample format.
- DoP marker bytes enter the processing chain as mutable PCM audio.
- Hi-Res support is claimed from file metadata without output path evidence.
- FFmpeg decode capability is advertised as native DSD or DoP passthrough.
- Shared-mode output is advertised as bit-perfect Hi-Res or DSD without truth
  evidence.
- A host ABI claim is added before core and adapter evidence exists.
- Any implementation file combines container probing, codec conversion, output
  negotiation, and claim evaluation in one file.

## Readiness Verdict

```text
KIVO-AUDIO-CORE-P0-Q-RESEARCH-FAMILY-LOCKED-001
HIRES_PCM_CONTRACT_AND_DSD_DOP_EXPLICIT_REJECTION_COMPLETE_PLAYBACK_NOT_AUTHORIZED
```
