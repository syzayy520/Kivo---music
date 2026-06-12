# Policy Index

**Version:** V10.1 FINAL P0-B READY LOCKED PATCHED
**Current Task:** KIVO-AUDIO-CORE-P0-P-RELEASE-HARDENING
**Last Updated:** 2026-06-12 (P0-P Release Hardening)

## 1. Authority

V10.1 remains the architecture constitution. The execution roadmap defines
the implementation order from the current repository state.

- Roadmap: `docs/architecture/audio_core_execution_roadmap.md`
- Release contract: `docs/architecture/commercial_release_contract.md`
- Active taskbook:
  `docs/tasks/p0_p_release_hardening_family_design_task.md`

Safety, legal, privacy, and project-root rules take precedence over schedule.

## 2. Phase Matrix

| Phase | Summary | Status |
|---|---|---|
| P0-B | Skeleton, governance, and policy | DONE |
| P0-C | Core contract foundation | DONE |
| P0-D/P0-E | Deterministic render proof | DONE |
| P0-F | WASAPI adapter shell | DONE |
| P0-G | Decode contract and FFmpeg boundary | DONE |
| P0-H | Minimum real WASAPI output | DONE |
| P0-I | FFmpeg-to-WASAPI loop | DONE |
| P0-J | State machine hardening | DONE |
| P0-K | Commercial stability | DONE |
| P0-L | Gapless playback | DONE |
| P0-M | Exclusive mode and bit-perfect truth | DONE |
| P0-N | ReplayGain, volume, resample, and DSP | DONE |
| P0-O | Host ABI | DONE |
| P0-P | Commercial release hardening | ACTIVE |
| P0-Q | Hi-Res, DSD, and DoP research | PLANNED |
| P0-R | mpv/libmpv reference research | PLANNED |

## 3. Active Gate Set

| Gate | Ownership | Status requirement |
|---|---|---|
| Forbidden Token Gate | All phases | PASS |
| Genealogy Gate | All phases | PASS |
| Contract Genealogy Gate | P0-C+ | PASS |
| No Empty Forest Gate | All phases | PASS |
| Authorized Audio Runtime Gate | P0-C+ | PASS |
| Policy Substance Gate | All phases | PASS |
| Dependency and License Gate | P0-G+ | PASS |
| Toolchain and Configure Gate | All phases | PASS |
| Audio Core Scope Lock Gate | P0-C+ | PASS |

P0-P additionally requires full Release build, CTest, binary reproducibility,
package verification, SBOM, notices, privacy review, and release matrices.

## 4. Release Blocker Registry

The active release matrix records:

- Authenticode certificate and signed product installer.
- Exact FFmpeg corresponding-source archive.
- External commercial legal approval.
- Product installer clean install, atomic upgrade, and rollback.
- USB DAC, Bluetooth, active HDMI, physical transition, and endurance rows.

Open external rows block commercial approval. They do not convert a passing
engineering candidate into a source failure.

## 5. Release Classifications

- `ENGINEERING_RC_READY_COMMERCIAL_RELEASE_BLOCKED`: repository-controlled
  evidence passes while named external gates remain open.
- `COMMERCIAL_RELEASE_APPROVED`: all engineering and external gates pass.
- `RELEASE_ENGINEERING_FAILED`: build, test, gate, reproducibility, package,
  privacy, compliance, or ABI verification fails.

## 6. P0-P Handoff

P0-P owns the reproducible engineering release-candidate payload and its
evidence. P0-Q and P0-R remain research-only and may not change current product
claims without a new implementation phase and evidence review.

## 7. Project Root Rule

- `PROJECT_ROOT` is `KivoAudioCoreCpp/`.
- Work must remain inside this project root.
- Product UI, Rust backend, and external installer code are separate owners.

## 8. Immediate Execution Order

1. Generate and verify the unsigned engineering release candidate.
2. Retain the exact FFmpeg corresponding-source archive.
3. Complete external legal approval.
4. Integrate and sign the product installer.
5. Complete remaining physical-device and endurance rows.
6. Approve commercial capability claims only after all evidence is stored.
