# KivoAudioCoreCpp

Kivo Windows Desktop Commercial Audio Core — C++ Implementation

## Project Status

**Current Phase:** P0-D (Validation Hardening and Contract Correctness Repair)

This project is the standalone C++ audio core for the Kivo Windows desktop music player.
It replaces the previous Rust audio backend with a fully self-controlled C++ implementation.

Current state:

- P0-C contract cleanup is ready for closeout.
- P0-D is hardening validation, public-header composability, and core contract correctness.
- There is still no production audio runtime engine in this repository.
- FFmpeg decode, WASAPI output, realtime render thread, and end-to-end playback are future implementation phases.

## Architecture Direction

- **Output Layer:** Windows WASAPI (shared/exclusive/bit-perfect)
- **Decode Layer:** FFmpeg
- **Reference:** mpv/libmpv as reference/benchmark/fallback research only
- **Target:** Windows x64, MSVC toolchain, C++20

## Phase Roadmap

| Phase | Name | Status |
|-------|------|--------|
| P0-A | Design Planning Lock | DONE |
| P0-B | Skeleton + Governance + Policy Pack | DONE |
| P0-C | Core Contract Foundation | CLOSEOUT |
| P0-D | Validation Hardening + Contract Correctness Repair | IN PROGRESS |
| P0-E | Minimal Decode / Output Seam Planning | PLANNED |
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

## Build and Validation

P0-D has contract build/test targets and validation tooling.

Lightweight local smoke check:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\validation\git_state_probe.ps1
powershell -ExecutionPolicy Bypass -File .\tools\validation\environment_probe.ps1
powershell -ExecutionPolicy Bypass -File .\tools\validation\validation_smoke_entry.ps1
```

Full local validation requires a Windows C++ toolchain shell, such as:

```text
x64 Native Tools Command Prompt for VS
Developer PowerShell for VS
```

See:

```text
docs/validation/windows_toolchain_setup.md
docs/validation/p0_d_validation_tooling_usage.md
```

## Policy

See `docs/architecture/` for the full policy pack.

## License

Proprietary — Kivo Music
