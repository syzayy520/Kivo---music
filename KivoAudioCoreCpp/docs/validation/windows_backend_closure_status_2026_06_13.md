# Windows Backend Closure Status - 2026-06-13

**Status:** REPOSITORY IMPLEMENTATION ACTIVE, COMMERCIAL EVIDENCE OPEN

## Repository-Controlled Status

| Workstream | State | Evidence |
|---|---|---|
| PR #1 description truth | DONE | PR no longer claims docs-only or build-blocked scope |
| P0-Q DSD/DoP truth | DONE | Explicit DSD identity, rejection and DoP transport contracts |
| P0-R mpv reference truth | DONE | Null-output probe and reproducibility requirements |
| Production diagnostics | IMPLEMENTED | Additive host ABI counters and compatibility tests |
| Installer payload transactions | IMPLEMENTED | Clean, upgrade, rollback and tamper self-test |
| Device inventory evidence | IMPLEMENTED | UTF-8 JSON endpoint inventory and ten required lab rows |
| Ten-hour evidence tooling | IMPLEMENTED | Resource sampling and runtime counter classification |
| Commercial regression workflow | IMPLEMENTED | Privacy-safe intake and closure validation |
| Lyrics ownership | LOCKED | Independent `KivoLyricsCoreCpp` sibling project |

## Current Local Evidence

- Clean MSVC x64 Release build completed 695 build steps.
- Full CTest passed 18/18 targets.
- All nine repository gates passed after explicit family-tree registration.
- Installer verifier passed clean, upgrade and rollback fixtures and rejected
  a tampered payload.
- Current development host inventory is valid with 12 discovered endpoints.
- All ten physical commercial rows remain `not_run`; none is claimed passed.
- No ten-hour endurance run was executed during this change.

## External Commercial Gates

| Gate | Truthful state | Required completion evidence |
|---|---|---|
| Authenticode | BLOCKED | Final DLL and installer signatures with RFC 3161 timestamp |
| FFmpeg source custody | OPEN | Exact corresponding source/build-script archive and hash |
| Legal approval | BLOCKED | Signed market/codec/LGPL review record |
| Product installer | BLOCKED | Signed clean VM install, upgrade, rollback and uninstall logs |
| USB DAC class 1/2 | OPEN | Physical rows with driver and format evidence |
| Bluetooth A2DP | OPEN | Reconnect/profile transition evidence |
| Active HDMI | OPEN | Connected display audio playback evidence |
| Device transitions | OPEN | Switch, unplug, disable, restart and sleep/resume evidence |
| Ten-hour endurance | OPEN | Physical ten-hour run with device and resource evidence |
| User feedback | OPEN | Real issue-to-regression records from commercial use |

## Next Execution Order

1. Produce a new release candidate and update PR #1 audit evidence.
2. Obtain signing identity and sign final DLL/installer bytes.
3. Archive exact FFmpeg corresponding source and attach legal approval.
4. Run clean install, upgrade, rollback and uninstall on clean Windows VMs.
5. Execute every physical device row and both required endurance modes.
6. Start controlled rollout using the commercial regression record gate.

## LyricsCore Decision

Lyrics must remain an independent module. Parser, provider, cache, alignment,
translation and lyric text diagnostics do not belong in AudioCore. Product
orchestration joins LyricsCore timeline selection to versioned AudioCore
position snapshots off realtime threads.
