# KivoAudioCoreCpp Design Planning 001

**Version:** V10.1 FINAL P0-B READY LOCKED PATCHED  
**Type:** TXT / planning-review / p0b-ready-lock / final-actionable  
**Task:** KIVO-CPP-AUDIO-CORE-P0-B-SKELETON-GOVERNANCE-POLICY-LEAN-004  

## 1. Final Ruling

V10 mainline is established and can proceed to P0-B. V10.1 does not overturn V10, does not change the long-term roadmap, does not change the P0-B task name, and does not add new long-range modules.

V10.1 only corrects 7 execution risk points:

1. P0-B is "commit + push" mode by default (unless no remote or toolchain environment blocked).
2. Closeout does not write final commit hash; the final delivery report must include the complete 40-char hash.
3. CMakePresets.json uses CMake 3.24-compatible schema version 5.
4. CMakePresets does not fix architecture unless using Visual Studio generator.
5. Forbidden token gate uses layered scanning: docs/policy/gate list may describe bans; source/runtime implementation has zero tolerance.
6. Gate cannot use full-repo keyword brute-scan causing false positives; cannot miss real runtime pollution.
7. Files outside PROJECT_ROOT must never be modified; if uncertain about execution location, must STOP.

## 2. Architecture Direction

- Rust frozen as specification asset
- C++ independent audio core
- Windows desktop mainline: WASAPI
- Decode mainline: FFmpeg
- mpv/libmpv: reference/benchmark/fallback research only

## 3. Phase Roadmap

| Phase | Name | Allowed |
|-------|------|---------|
| P0-B | Skeleton + Governance + Policy | Minimum skeleton, policy pack, docs/closeout, smoke gates, configure-only CMake |
| P0-C | Core Contract Foundation | core/contract, tests/contracts, contract build/test harness |
| P0-D | Fake Backend Contract Proof | tests/fakes, fake backend family |
| P0-E | Render Boundary Contract | RenderClientBoundary trait |
| P0-F | WASAPI Adapter Shell | platform/wasapi adapter shell |
| P0-G | Decode Contract + FFmpeg Seam | decode/contract, FFmpeg seam |
| P0-H | Minimum Real WASAPI Output | Real WASAPI output, playback thread |
| P0-I | Minimum FFmpeg Decode -> WASAPI Loop | Full decode-output loop |
| P0-J | State Machine Hardening | State machine, error recovery |
| P0-K | Commercial Stability | Device matrix, corrupted corpus |
| P0-L | Gapless Playback | Sample-accurate gapless |
| P0-M | Exclusive Mode | Bit-perfect, exclusive mode |
| P0-N | ReplayGain / DSP | Volume, DSP, resampler boundary |
| P0-O | Host ABI | C ABI, Tauri integration |
| P0-P | Commercial Release | Installer, signing, crash diagnostics |
| P0-Q | Hi-Res Research | DSD, DoP, decode isolation |
| P0-R | mpv Reference Research | mpv benchmark/fallback |

## 4. Execution Basis

This document is the sole execution basis for P0-B.

**Iron Rule 40:** P0-B can only build minimum skeleton, policy pack, docs/closeout, gate smoke, and configure-only CMake. No contracts, fake backends, WASAPI, FFmpeg, playback threads, or complete directory trees.
