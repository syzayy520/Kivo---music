# Policy Index

**Version:** V10.1 FINAL P0-B READY LOCKED PATCHED  
**Current Task:** KIVO-CPP-AUDIO-CORE-P0-C-CLOSEOUT-HARDENING
**Last Updated:** 2026-06-12 (Execution Roadmap Activation)

## 1. Version Declaration

This policy index is based on V10.1 FINAL P0-B READY LOCKED PATCHED. V10.1 does not overturn V10; it only corrects 7 execution risk points.

## 2. Current Task Name

KIVO-CPP-AUDIO-CORE-P0-C-CLOSEOUT-HARDENING

## 2.1 Active Execution Authority

**Document:** `docs/architecture/audio_core_execution_roadmap.md`
**Document ID:** KIVO-CPP-AUDIO-CORE-EXECUTION-ROADMAP-001
**Status:** ACTIVE EXECUTION BASELINE

V10.1 remains the architecture constitution. The execution roadmap defines
the implementation order from the current repository state. When an older
planning document names a different next action, the execution roadmap takes
precedence unless a safety, legal, privacy, or project-root rule from V10.1
would be violated.

## 3. Phase Matrix

| Phase | Summary | Allowed | Forbidden |
|-------|---------|---------|-----------|
| P0-B | Skeleton + Governance + Policy | skeleton, policy pack, docs, gates, configure-only CMake | src/, tests/, contracts, fake backends, WASAPI, FFmpeg |
| P0-C | Core Contract Foundation | contract correctness, header composition, tests | WASAPI, FFmpeg, mpv, fake backends, playback |
| P0-D/P0-E | Deterministic Render Proof | minimal render boundary, fake clock/backend, fault injection | Real platform, real decode |
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
| Contract Genealogy Gate | P0-C+ | PASS |
| No Empty Forest Gate | All phases | SMOKE |
| No Audio Runtime Gate | P0-B | SMOKE |
| Policy Substance Gate | P0-B | SMOKE |
| Dependency/License Gate | P0-B+ | SMOKE |
| Toolchain/Configure Gate | P0-B | SMOKE |
| Audio Core Scope Lock Gate | P0-012+ | PASS |

## 5. Future Blocker Registry

Future blocker registry is maintained in the blocker registry section of the design planning document. P0-B does NOT implement future gate scripts as scripts — they are documented only.

## 6. Go / No-Go Rules

- **GO_TO_P0_C:** All gates PASS, git clean, push status clear
- **NO_GO_***: Various failure states that block P0-C progression
- No NO_GO state may proceed to P0-C without explicit project owner sign-off

## 7. P0-C Handoff

P0-C contract targets and tests exist. P0-C remains in closeout hardening until
public headers compose, audio-format truth is internally consistent, generated
directories are excluded from gates, and the supported validation path can run
configure, build, tests, and gates from a clean environment.

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

## 10. Immediate Execution Order

1. Close P0-C correctness and validation gaps.
2. Define the minimal platform-neutral render boundary.
3. Implement deterministic fake clock/backend fault injection.
4. Implement the WASAPI shell and real generated-PCM output.
5. Implement the layered FFmpeg seam and local-file playback loop.
6. Harden state, seek, command races, generations, and recovery.
7. Continue through stability, gapless, bit-perfect, processing, ABI, and
   release phases as defined by the active execution roadmap.
