# KivoAudioCoreCpp

Kivo Windows Desktop Commercial Audio Core — C++ Implementation

## Project Status

**Current execution position:** P0-P commercial release hardening.

This project is the standalone C++ audio core for the Kivo Windows desktop music player.
It replaces the previous Rust audio backend with a fully self-controlled C++ implementation.

Current state:

- Public contracts and deterministic render/failure boundaries are complete.
- Event-driven WASAPI shared output is implemented and hardware-verified.
- FFmpeg local-file decode, bounded queue transfer, playback control, seek,
  recovery, hostile-media handling, and gapless timeline behavior are present.
- WASAPI exclusive negotiation is implemented without shared fallback.
- Bit-perfect is reported only from complete mode, format, engine, conversion,
  and processing evidence.
- ReplayGain, software volume, deterministic dither, explicit resample
  quality, conversion latency/tail truth, and strict bypass are complete.
- Versioned C ABI, opaque handles, host callback sources, sanitized
  diagnostics, and Tauri adapter ownership rules are complete.
- P0-P release hardening is the active phase.

The authoritative implementation order is:

[`docs/architecture/audio_core_execution_roadmap.md`](docs/architecture/audio_core_execution_roadmap.md)

Future work must follow that roadmap unless an explicit, owner-approved
roadmap amendment is committed.

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
| P0-C | Core Contract Foundation | DONE |
| P0-D/P0-E | Validation + Deterministic Render Proof | DONE |
| P0-F | WASAPI Adapter Shell | DONE |
| P0-G | Decode Contract + FFmpeg Seam | DONE |
| P0-H | Minimum Real WASAPI Output | DONE |
| P0-I | Minimum FFmpeg Decode -> WASAPI Loop | DONE |
| P0-J | State Machine Hardening | DONE |
| P0-K | Commercial Stability / Device Matrix | DONE |
| P0-L | Gapless Playback / Timeline Accuracy | DONE |
| P0-M | Exclusive Mode / Bit-Perfect | DONE |
| P0-N | ReplayGain / Volume / DSP | DONE |
| P0-O | Host ABI / Tauri Integration | DONE |
| P0-P | Commercial Release Hardening | ACTIVE |
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
