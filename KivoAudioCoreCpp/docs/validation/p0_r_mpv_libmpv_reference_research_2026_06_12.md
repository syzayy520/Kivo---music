# P0-R mpv and libmpv Reference Research Notes

**Date:** 2026-06-12
**Status:** REFERENCE PROBE INITIALIZED

## Findings

- mpv/libmpv is useful as a behavioral reference and compatibility candidate,
  but it is not a transparent substitute for the Kivo playback core.
- libmpv integration would create a separate playback owner with its own
  lifecycle, option policy, logging, event loop, binary provenance, and
  licensing obligations.
- A shipped compatibility mode must be labeled separately and excluded from
  primary core truth claims.
- External-process isolation should be evaluated before embedded libmpv,
  because it creates a cleaner failure and license boundary.

## Current Product Claim Impact

No current product claim changes.

Forbidden claims remain:

- mpv/libmpv compatibility mode.
- mpv-backed bit-perfect playback.
- mpv-backed gapless proof.
- mpv-backed Hi-Res, DSD, or DoP support.
- mpv as a silent fallback.

## Primary References

- libmpv client API:
  `https://mpv.io/manual/master/#client-api`
- mpv manual:
  `https://mpv.io/manual/master/`
- mpv source:
  `https://github.com/mpv-player/mpv`

## Next Research Slice

Recommended first implementation-adjacent slice:

```text
Reference-only command-line benchmark with pinned options
```

Reason:

- It requires no core linkage.
- It produces useful comparison evidence for decode success, startup latency,
  seek behavior, and hostile media handling.
- It avoids shipping libmpv before license, binary provenance, and UX policy
  are complete.

Required evidence:

- Pinned mpv version and binary provenance.
- Full command line and option policy.
- Test media checksums and license status.
- Separate result labels from Kivo core validation.
- Explicit statement that benchmark pass does not prove Kivo core behavior.

## Completed Local Evidence

The repository now includes `tools/validation/mpv_reference_probe.ps1`.
The active gate runner also includes
`tools/gates/check_mpv_reference_probe_gate.ps1` to statically lock the probe
options and classification labels without requiring a local mpv install.

Probe classifications:

- `BLOCKED_BY_LOCAL_MPV_INSTALL`: mpv is not installed or not supplied.
- `PASS_MPV_REFERENCE_VERSION_ONLY`: mpv version can be queried, but no media
  was supplied.
- `BLOCKED_BY_LOCAL_REFERENCE_MEDIA`: requested media path is missing.
- `PASS_MPV_REFERENCE_PLAYBACK`: reference-only decode/playback command
  completed with `--ao=null`.
- `FAIL_MPV_REFERENCE_PLAYBACK`: mpv failed on the reference media.

This probe is not part of normal Kivo core validation and does not prove Kivo
decode, render, gapless, bit-perfect, Hi-Res, DSD, or DoP behavior.
