# Policy Index

**Version:** V10.1 FINAL P0-B READY LOCKED PATCHED  
**Current Task:** KIVO-CPP-AUDIO-CORE-P0-B-SKELETON-GOVERNANCE-POLICY-LEAN-004  
**Last Updated:** Post-P0B enhancement input planning  

## 1. Version Declaration

This policy index is based on V10.1 FINAL P0-B READY LOCKED PATCHED. V10.1 does not overturn V10; it only corrects 7 execution risk points.

## 2. Current Task Name

KIVO-CPP-AUDIO-CORE-P0-B-SKELETON-GOVERNANCE-POLICY-LEAN-004

## 3. Phase Matrix

| Phase | Summary | Allowed | Forbidden |
|-------|---------|---------|-----------|
| P0-B | Skeleton + Governance + Policy | skeleton, policy pack, docs, gates, configure-only CMake | src/, tests/, contracts, fake backends, WASAPI, FFmpeg |
| P0-C | Core Contract Foundation | core/contract, tests/contracts | WASAPI, FFmpeg, mpv, fake backends, playback |
| P0-D | Fake Backend Contract Proof | tests/fakes, fake backend family | Real platform, real decode |
| P0-E | Render Boundary Contract | RenderClientBoundary trait | Real WASAPI |
| P0-F | WASAPI Adapter Shell | platform/wasapi shell | Real audio output |
| P0-G | Decode Contract + FFmpeg Seam | decode/contract, FFmpeg seam | Real decode |
| P0-H | Minimum Real WASAPI Output | Real WASAPI, playback thread | FFmpeg, mpv |
| P0-I | Min FFmpeg->WASAPI Loop | Full decode-output loop | Gapless, DSP |
| P0-J | State Machine Hardening | Error recovery, state machine | New features |
| P0-K | Commercial Stability | Device matrix, corpus | New features |
| P0-L | Gapless Playback | Sample-accurate gapless | Exclusive mode |
| P0-M | Exclusive Mode | Bit-perfect, exclusive | DSP |
| P0-N | ReplayGain / DSP | Volume, DSP, resampler | Hi-Res |
| P0-O | Host ABI | C ABI, Tauri | New features |
| P0-P | Commercial Release | Installer, signing | New features |
| P0-Q | Hi-Res Research | DSD, DoP, isolation | Implementation |
| P0-R | mpv Reference Research | mpv benchmark | Integration |

## 4. Gate Ownership Matrix

| Gate | Owner Phase | P0-B Status |
|------|-------------|-------------|
| Forbidden Token Gate | All phases | SMOKE |
| Genealogy Gate | All phases | SMOKE |
| No Empty Forest Gate | All phases | SMOKE |
| No Audio Runtime Gate | P0-B | SMOKE |
| Policy Substance Gate | P0-B | SMOKE |
| Dependency/License Gate | P0-B+ | SMOKE |
| Toolchain/Configure Gate | P0-B | SMOKE |

## 5. Future Blocker Registry

Future blocker registry is maintained in the blocker registry section of the design planning document. P0-B does NOT implement future gate scripts as scripts — they are documented only.

## 6. Go / No-Go Rules

- **GO_TO_P0_C:** All gates PASS, git clean, push status clear
- **NO_GO_***: Various failure states that block P0-C progression
- No NO_GO state may proceed to P0-C without explicit project owner sign-off

## 7. P0-C Handoff

P0-C first step: extend P0-B configure-only CMake to contract build/test harness. Add contract library target, contract test target. Only compile core contract. Only run tests/contracts.

## 8. PROJECT_ROOT Rule

- PROJECT_ROOT = KivoAudioCoreCpp/ (subdirectory of repo root)
- Only files within PROJECT_ROOT may be modified
- No modification of src-tauri/, React frontend, Rust audio backend, freeze docs, or any existing files outside PROJECT_ROOT

## 9. Enhancement Input Reference

**Document:** `docs/architecture/p0c_through_p0i_enhancement_input.md`  
**Task:** KIVO-CPP-AUDIO-CORE-POST-P0B-P0C-P0I-CONTRACT-FOUNDATION-ENHANCEMENT-INPUT-002  
**Type:** planning-only / post-p0b-review / next-phase-task-authoring-input  
**Status:** COMPLETED

**Coverage:**
- P0-C: 14 contract families with full type/responsibility/constraint specification
- P0-D: Fault injection system (18 injection capabilities)
- P0-E: Failure-first testing (14 failure scenarios)
- P0-F: WASAPI adapter shell (12 reservation items)
- P0-G: Decode contract + FFmpeg seam (11 seam layers)
- P0-H/P0-I: Anti-demo requirements
- P0-K through P0-P: Deferred but reserved
- mpv fallback boundary
- NAS/WebDAV/cloud drive source boundary

**Go / No-Go:** GO_TO_P0_C_TASK_AUTHORING
