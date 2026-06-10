# KivoAudioCoreCpp

Kivo Windows Desktop Commercial Audio Core — C++ Implementation

## Project Status

**Current Phase:** P0-B (Skeleton Governance, Policy Pack, Configure-Only CMake)

This project is the standalone C++ audio core for the Kivo Windows desktop music player.
It replaces the previous Rust audio backend with a fully self-controlled C++ implementation.

## Architecture Direction

- **Output Layer:** Windows WASAPI (shared/exclusive/bit-perfect)
- **Decode Layer:** FFmpeg
- **Reference:** mpv/libmpv as reference/benchmark/fallback research only
- **Target:** Windows x64, MSVC toolchain, C++20

## Phase Roadmap

| Phase | Name | Status |
|-------|------|--------|
| P0-A | Design Planning Lock | DONE |
| P0-B | Skeleton + Governance + Policy Pack | IN PROGRESS |
| P0-C | Core Contract Foundation | NEXT |
| P0-D | Fake Backend Contract Proof | PLANNED |
| P0-E | Render Boundary Contract | PLANNED |
| P0-F | WASAPI Adapter Shell | PLANNED |
| P0-G | Decode Contract + FFmpeg Seam | PLANNED |
| P0-H | Minimum Real WASAPI Output | PLANNED |
| P0-I | Minimum FFmpeg Decode -> WASAPI Loop | PLANNED |
| P0-J | State Machine Hardening | PLANNED |
| P0-K | Commercial Stability / Device Matrix | PLANNED |
| P0-L | Gapless Playback / Timeline Accuracy | PLANNED |
| P0-M | Exclusive Mode / Bit-Perfect | PLANNED |
| P0-N | ReplayGain / Volume / DSP | PLANNED |
| P0-O | Host ABI / Tauri Integration | PLANNED |
| P0-P | Commercial Release Hardening | PLANNED |
| P0-Q | Hi-Res / DSD / DoP Research | PLANNED |
| P0-R | mpv/libmpv Reference Research | PLANNED |

## Build

P0-B is configure-only. No build targets, no runtime targets, no test targets.

```powershell
cmake --preset p0b-configure-smoke
```

## Policy

See `docs/architecture/` for the full policy pack.

## License

Proprietary — Kivo Music
