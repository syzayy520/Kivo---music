# P0-R mpv and libmpv Reference Research

**Task:** KIVO-CPP-AUDIO-CORE-P0-R-MPV-LIBMPV-REFERENCE-RESEARCH
**Status:** REFERENCE PROBE INITIALIZED
**Execution Basis:** P0-P engineering RC closeout and execution roadmap
**Last Updated:** 2026-06-12

## Goal

Research mpv and libmpv as a reference, benchmark, and possible isolated
compatibility mode without weakening the primary Kivo audio core. P0-R must
not turn mpv into the core playback engine, a hidden fallback, or evidence for
Kivo bit-perfect, gapless, or Hi-Res claims.

## Base Gate

- P0-P engineering RC evidence remains intact.
- Primary playback continues to use the Kivo core, FFmpeg adapter, processing
  boundary, and WASAPI renderer.
- mpv and libmpv remain outside core contracts and current release claims.
- Any future compatibility mode must be opt-in, labeled, isolated, observed,
  licensed, and excluded from primary audio-truth claims.

## Inventory Gate

- mpv has a documented client API for embedding and control through libmpv.
- mpv also has a large command and option surface that can change playback,
  filtering, output, timing, buffering, and format decisions.
- libmpv cannot be treated as a transparent substitute for the current
  renderer, decoder, queue, state machine, or bit-perfect evaluator.
- mpv license, binary provenance, codec distribution, scripting, and option
  policy need a separate release audit before shipping.

## Research Evidence

Primary references captured for this phase:

- libmpv client API: `https://mpv.io/manual/master/#client-api`
- mpv options and audio output behavior: `https://mpv.io/manual/master/`
- mpv source and headers: `https://github.com/mpv-player/mpv`

These references are research inputs only. A future shipped compatibility mode
requires local binary provenance, license review, configuration lock, process
isolation review, and product UX approval.

## Natural Family Tree

Research ownership:

```text
docs/tasks/
  p0_r_mpv_libmpv_reference_research_family_design_task.md
docs/validation/
  p0_r_mpv_libmpv_reference_research_2026_06_12.md
```

Reference-only local tooling ownership:

```text
tools/validation/
  mpv_reference_probe.ps1
tools/gates/
  check_mpv_reference_probe_gate.ps1
```

Future implementation, if approved, must stay outside the primary core:

```text
src/compatibility/mpv/
  process/          # external process mode, if selected
  client/           # libmpv client mode, if selected
  policy/           # allowed options and feature clamps
  observation/      # compatibility-only telemetry
  lifecycle/        # startup, shutdown, and failure mapping

include/kivo/compatibility/mpv/
  capability/
  diagnostic/
  result/

tests/compatibility/mpv/
  fixture/
  runner/
  scenario/
```

If the repository keeps compatibility outside `KivoAudioCoreCpp`, this tree
must live in the owning product repository instead. It must never be merged
into `src/core`, `include/kivo/core`, `src/platform/windows/wasapi`, or
`src/adapters/ffmpeg`.

## Responsibility Gate

- Reference research owns behavioral comparison only.
- Compatibility policy owns the allowed mpv options and disabled features.
- Process/client integration owns mpv lifecycle only.
- Observation owns compatibility-mode counters and error mapping only.
- Product host owns UI labeling, consent, and user selection.
- Release compliance owns mpv binary provenance, license notices, scripts,
  codecs, and distribution obligations.
- Primary core truth evaluator owns Kivo playback claims only and must ignore
  mpv compatibility success.

## Implementation Admission Rules

P0-R implementation may proceed only as one narrow slice at a time:

1. Reference-only command-line benchmark with documented media and options.
2. External-process compatibility prototype with no core linkage.
3. libmpv embedded prototype behind an isolated compatibility adapter.
4. License and distribution audit for a pinned mpv binary.
5. Product UX contract for "Compatibility Mode" or "Emergency Playback Mode".

Each slice must include isolation, option policy, lifecycle failure handling,
and claim review. No slice may use mpv playback success as proof that Kivo core
decode, render, gapless, bit-perfect, or processing behavior works.

## Completed Local Slice

The repository includes `tools/validation/mpv_reference_probe.ps1` for a
reference-only command-line probe. It reports the discovered executable
version and uses a locked option set:

```text
--no-config --no-video --ao=null --ao-null-untimed
```

The probe never links libmpv, never enters core, and reports missing mpv or
missing local media as local blockers rather than source failures.

The active gate runner includes `tools/gates/check_mpv_reference_probe_gate.ps1`
to statically lock the probe to reference-only null-output, untimed options and
classification labels without requiring a local mpv install.

The executable version itself is not pinned until a reviewed binary and its
hash/provenance record are supplied. Therefore this slice is a probe, not a
reproducible benchmark baseline.

## STOP Conditions

- `mpv` or `libmpv` headers are included from core, render, decode, processing,
  output-truth, or host ABI families.
- mpv is used as a silent fallback after Kivo core failure.
- mpv playback is counted as bit-perfect, gapless, Hi-Res, DSD, or DoP proof.
- mpv options are left unrestricted in a shipped compatibility mode.
- mpv binary provenance, license, scripting, or codec obligations are missing.
- Compatibility telemetry shares primary-core counters without a source label.

## Readiness Verdict

```text
KIVO-AUDIO-CORE-P0-R-RESEARCH-FAMILY-LOCKED-001
REFERENCE_PROBE_INITIALIZED_COMPATIBILITY_IMPLEMENTATION_NOT_AUTHORIZED
```
