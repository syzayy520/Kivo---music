# P0-M Exclusive and Bit-Perfect Truth

**Task:** KIVO-CPP-AUDIO-CORE-P0-M-EXCLUSIVE-BIT-PERFECT-TRUTH
**Status:** COMPLETED
**Execution Basis:** P0-L closeout and execution roadmap
**Last Updated:** 2026-06-12

## Goal

Implement WASAPI exclusive output independently from shared output and permit a
bit-perfect claim only when every required path and mutation fact is known and
clean.

## Base Gate

- P0-L full build, CTest, and repository gates pass.
- Shared event-driven WASAPI output is stable and remains the default.
- Output truth contracts already reserve format, processing, transparency, and
  engine-participation vocabulary.
- P0-M may strengthen truth behavior but must not implement P0-N processing.

## Inventory Gate

- `RenderOpenRequest` has format policy but no requested access mode.
- WASAPI currently always opens the endpoint mix format in shared mode.
- `WasapiDiagnostics` already exposes actual mode but reports exclusive support
  as false.
- Existing bit-perfect structs are mutable value carriers; no production
  evaluator prevents incomplete evidence from becoming a claim.

## ODR Gate

- Reuse `RenderFormat`, `ConversionPolicy`, `ResampleDecision`,
  `ProcessingParticipation`, and existing output-truth contracts.
- Add one render access-mode enum. Do not reuse marketing/result states as an
  API initialization mode.
- Do not introduce another renderer, device-format type, or processing chain.
- Core truth code must not include Windows, WASAPI, COM, or FFmpeg headers.

## Natural Family Tree

```text
include/kivo/core/output/truth/
  evaluation/
  evidence/
  result/

src/core/output/truth/
  evaluation/

tests/output_truth/
  fixture/
  runner/
  scenario/
    evaluation/
```

WASAPI work remains with its existing owners:

```text
include/kivo/core/render/format/
include/kivo/platform/windows/wasapi/diagnostics/
src/platform/windows/wasapi/
  format/
  renderer/
tests/platform_windows/wasapi/
  scenario/
```

## Responsibility Gate

- Render format owns requested Shared/Exclusive access only.
- WASAPI format owns conversion to and from `WAVEFORMATEX`.
- WASAPI renderer open owns mode-specific probing and initialization.
- Diagnostics owns the actual mode and event-driven capability facts.
- Evidence owns optional facts only.
- Evaluator owns deterministic truth priority only.
- Result owns the immutable evaluation output only.
- No implementation or test file may exceed 260 lines.

## Exclusive Proof

- Shared mode continues to use the endpoint mix format.
- Exclusive mode uses the requested render format exactly.
- Exclusive mode rejects `AllowCompatible`; no silent fallback to shared or a
  closest format is permitted.
- `IAudioClient::IsFormatSupported(EXCLUSIVE)` must return exact support before
  initialization.
- Event-driven exclusive initialization uses a valid device period for both
  buffer duration and periodicity.
- Diagnostics report the actual initialized mode, never merely the request.

## Bit-Perfect Proof

Required known evidence:

- Requested and actual output access mode.
- Source, render, and device format.
- Windows audio-engine participation.
- Resampling, sample encoding conversion, bit-depth conversion, channel remix.
- ReplayGain, software volume, DSP, and dither/sample mutation.
- User policy permission.

The evaluator rejects in a stable priority order. Incomplete evidence,
shared-mode output, or requested/actual mode disagreement can never be active.

## Tests

- Render request defaults to Shared and validates Exclusive.
- Core evaluator accepts only complete, exact, exclusive, mutation-free proof.
- Every required mutation fact has an explicit rejection test.
- Unknown evidence is rejected.
- Shared mode is rejected honestly even when formats match.
- WASAPI reverse format mapping preserves valid bits, container bits, channels,
  channel mask, and sample rate.
- WASAPI capability declaration reports both event-driven modes.
- Existing shared hardware smoke remains unchanged.

## Completion Evidence

- Shared and exclusive initialization are separate renderer responsibilities.
- Exclusive initialization probes the exact requested extensible format and
  retries only for WASAPI buffer alignment; it never changes format or mode.
- Reverse format mapping round-trips Float32, Int16, Int24-in-32, and Int32.
- The core evaluator rejects incomplete evidence, policy denial, shared mode,
  mode disagreement, format mismatch, engine participation, conversion,
  resampling, remix, gain, volume, DSP, and dither.
- Output-truth tests pass 7 of 7.
- Contract tests pass 892 of 892.
- Render-boundary tests pass 22 of 22.
- Fake-renderer tests pass 26 of 26.
- WASAPI adapter tests pass 21 of 21.
- The default integrated endpoint completed the shared event-driven smoke with
  4,800 submitted and consumed frames, zero wait timeouts, zero underruns, and
  zero device invalidations.
- The same endpoint reported exact 48 kHz stereo Float32 exclusive support as
  unavailable. The renderer returned `AUDCLNT_E_UNSUPPORTED_FORMAT`, preserved
  the platform code, and did not fall back to shared mode.
- Largest P0-M core truth file is 147 lines; largest new WASAPI open-family
  file is 100 lines.

## STOP Conditions

- Exclusive silently falls back to shared.
- Closest-match format is accepted as bit-perfect.
- A missing fact defaults to clean.
- A core truth file includes a platform header.
- P0-N volume, ReplayGain, resampler, remix, or DSP implementation leaks into
  this phase.
- Hardware support is claimed without runtime probe or release-lab evidence.

## Readiness Verdict

```text
KIVO-AUDIO-CORE-P0-M-FAMILY-DESIGN-LOCKED-001
IMPLEMENTATION_COMPLETE_AND_VALIDATED
```
