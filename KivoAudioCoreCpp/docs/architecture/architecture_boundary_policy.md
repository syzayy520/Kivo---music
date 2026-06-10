# Architecture Boundary Policy

**Purpose:** Define strict architectural boundaries between core contracts, platform implementations, and runtime layers.  
**Scope:** All phases from P0-B onward.  
**Current Phase Rule:** P0-B has no implementation. Only policy documentation exists.  
**Last Updated:** P0-B (V10.1)  

## 1. Purpose

This policy ensures the audio core contract layer is never polluted by platform-specific implementations (WASAPI, FFmpeg, mpv). It defines the dependency direction and layer isolation rules.

## 2. Scope

Applies to all source code, tests, and documentation within KivoAudioCoreCpp/.

## 3. Current Phase Rule (P0-B)

P0-B has no implementation files. This policy documents the boundary rules for future phases.

## 4. Allowed

- Documentation describing layer boundaries
- Policy documents referencing WASAPI/FFmpeg/mpv as future phases
- Architecture diagrams showing dependency direction
- Core contract definitions (P0-C+)
- Platform adapter shells (P0-F+)
- Fake backend implementations for testing (P0-D+)

## 5. Forbidden

- Core contract headers including Windows.h, audioclient.h, mmdeviceapi.h
- Core contract headers including avcodec.h, avformat.h, avutil.h
- Core contract headers including mpv headers
- Platform implementation leaking into core layer
- Circular dependencies between layers
- Uncontrolled mutable global state across layers
- Hidden singleton patterns violating layer boundaries

## 6. Gate / Check Method

- `check_forbidden_token_gate.ps1` scans for platform implementation tokens in core layer
- `check_genealogy_gate.ps1` validates directory structure follows layer boundaries
- Layer violation is a HARD STOP

## 7. Failure Classification

- `FORBIDDEN_TOKEN_IMPLEMENTATION_POLLUTION_BLOCKER`: Platform token in core layer
- `ARCHITECTURE_BOUNDARY_VIOLATION_BLOCKER`: Dependency direction violated
- `LAYER_LEAK_BLOCKER`: Implementation leaking across layer boundary

## 8. Future Phase Ownership

- **P0-C:** Define core contract layer (time, result, audio_format, buffer, clock_contract, output_mode, resample_policy, device_writer)
- **P0-D:** Define fake backend layer (test-only, isolated from core)
- **P0-E:** Define render boundary contract
- **P0-F:** Define WASAPI adapter (depends on core, not vice versa)
- **P0-G:** Define FFmpeg decode seam (depends on core, not vice versa)

## 9. Deferred Items

- Formal dependency graph tooling
- Automated layer violation detection in CI

## 10. Layer Diagram

```
Host Application (Tauri/CLI)
        |
        v
   C ABI Boundary (P0-O)
        |
        v
   Audio Engine Core (P0-C to P0-N)
        |
    +---+---+
    |       |
    v       v
Platform   Decode
(P0-F)    (P0-G)
    |       |
    v       v
  WASAPI   FFmpeg
```

**Iron Rule 1:** Core contract can NEVER be polluted by WASAPI/FFmpeg/mpv.
