# P0-P Release Hardening Closeout

**Document ID:** KIVO-AUDIO-CORE-P0-P-CLOSEOUT-001
**Date:** 2026-06-12
**Source Commit:** `db4c640524a6ae5098e70b97c94a04d8281b6f66`
**Classification:** `ENGINEERING_RC_READY_COMMERCIAL_RELEASE_BLOCKED`

## Scope Completed

- Version `1.0.0` Windows resource metadata.
- Locked seven-function C ABI export table.
- MSVC reproducible-build flags.
- ASLR, NX, Control Flow Guard, and CET PE hardening.
- Deterministic Release PDB with private symbol separation.
- Two independent clean Release builds.
- Runtime and symbol payload staging.
- FFmpeg notices, source-retention information, and license text.
- CycloneDX 1.6 SBOM.
- Build, signing, reproducibility, installer, and release manifests.
- Runtime hash, export, PE, version, privacy, symbol, and signature checks.
- Deterministic runtime archive recreation check.
- Release matrix and commercial capability claim review.

## Validation Evidence

| Check | Result |
|---|---|
| Clean Release build A | PASS |
| Release CTest | PASS, 16/16 |
| Clean Release build B | PASS |
| Repository gates | PASS, 9/9 |
| Public C header checks | PASS |
| Locked ABI exports | PASS, 7 |
| DLL file/product version | PASS, 1.0.0 |
| PE hardening | PASS |
| Runtime PDB exclusion | PASS |
| Manifest path privacy | PASS |
| SBOM root/dependency structure | PASS |
| Engineering package verification | PASS |

## Reproducibility Evidence

Both independent builds produced the same `kivo_audio_core.dll`:

```text
aa673eabd162150f0fe70817aee06bedb8b05376704ac0fb000d2542cd72a976
```

The runtime archive was created twice from the same staged payload and both
archive hashes matched.

## Release Artifacts

Local output directory:

```text
out/release/1.0.0-rc.1/
```

| Artifact | SHA-256 |
|---|---|
| `kivo-audio-core-1.0.0-rc.1-win-x64.zip` | `8e9dd67008f0b6caf2a6316f231ccf1255b653936db5fbffa9a1a4333b50a739` |
| `kivo-audio-core-1.0.0-rc.1-symbols.zip` | `bd866bf23236fd6af2cc88d4375241548272a62b906307a3843e1735e797b614` |

The runtime archive contains the Kivo DLL, four FFmpeg DLLs, public C ABI
headers, license, notices, SBOM, and manifests. It contains no PDB. The symbols
archive contains the Kivo PDB.

## Dependency Evidence

- FFmpeg revision: `n7.1.4-39-ga5faeca88f-20260611`.
- Baseline archive SHA-256:
  `c8bc13fb4079fc477ecca83985d913d43d1a69efdb974ebc1f37490e603b5b79`.
- Linking: dynamic.
- Shipped FFmpeg DLLs: replaceable shared libraries.
- LGPL text, build configuration, upstream links, source-retention
  instructions, and per-file hashes are present.

## Commercial Blockers

Commercial release approval remains blocked by four external evidence groups:

1. No Authenticode code-signing certificate was available; the Kivo DLL is
   intentionally labeled unsigned.
2. Exact FFmpeg corresponding-source archive custody and external legal
   approval are not repository-controlled records.
3. USB DAC, Bluetooth, active HDMI, physical device transition, and ten-hour
   endurance release-lab rows remain open.
4. The product host installer must consume this payload and prove signed clean
   install, atomic upgrade, and rollback.

None of these rows is reported as passed.

## Final Decision

The C++ backend audio-core engineering scope through P0-P is complete and
produces a usable, verified engineering release candidate. Commercial shipping
is not approved until every external blocker above has stored evidence.

P0-Q Hi-Res/DSD/DoP and P0-R mpv/libmpv remain separate research phases and
are not required for the completed PCM audio-core release candidate.
