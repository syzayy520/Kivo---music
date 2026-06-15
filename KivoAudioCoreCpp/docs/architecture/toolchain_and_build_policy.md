# Toolchain and Build Policy

**Purpose:** Define the C++ toolchain, build system, and compilation standards for KivoAudioCoreCpp.  
**Scope:** All phases from P0-B onward.  
**Current Phase Rule:** P0-P engineering release-candidate validation requires clean configure, build, CTest, active gates, release evidence, and honest environment classification.
**Last Updated:** 2026-06-13

## 1. Purpose

This policy governs the C++ toolchain selection, build system configuration, and compilation standards. It ensures reproducible, auditable builds across development machines.

## 2. Scope

Applies to all CMakeLists.txt, CMakePresets.json, and any future build configuration files within KivoAudioCoreCpp/.

## 3. Current Phase Rule

The repository now contains active build, test, public-header, WASAPI, FFmpeg
boundary, release, and validation targets. Toolchain policy must therefore
distinguish configure smoke, full build/test validation, active gates, release
candidate validation, and local environment blockers.

P0-Q/P0-R research and contract work may add tests and probes, but it must not
claim DSD/DoP playback, mpv compatibility, or commercial support without a
later implementation slice and evidence review.

## 4. Allowed

- CMake 3.24 or newer.
- C++20 with extensions disabled.
- MSVC x64 environment import from ordinary PowerShell when available.
- Ninja Debug validation through `tools/validation/build_validation_entry.ps1`.
- Active gate validation through `tools/gates/run_cpp_audio_core_gates.ps1`.
- Release-candidate validation through release tooling when commercial claims,
  binaries, packages, signing, SBOM, notices, or provenance are in scope.
- Explicit classification of configure smoke, build/test pass, source failure,
  and local environment blocker.

## 5. Forbidden

- FetchContent (all phases)
- vcpkg/conan/toolchain file references (P0-B)
- Hardcoded absolute paths in CMakePresets
- Architecture pinning (unless Visual Studio generator explicitly needed)
- Claiming configure smoke as build, test, runtime, release, or commercial
  approval evidence.
- Reporting an environment blocker as PASS.
- Adding release or installer behavior without the release policy evidence.

## 6. Gate / Check Method

- `check_toolchain_configure_gate.ps1` validates CMake availability and runs configure-only smoke.
- `run_cpp_audio_core_gates.ps1` aggregates active repository gates and reports PASS, FAIL, or ENVIRONMENT_BLOCKED.
- `build_validation_entry.ps1` imports the MSVC environment, configures a clean build directory, builds all targets, and runs CTest.
- Release-candidate tooling owns reproducibility, signing, package, SBOM, notices, and release manifest checks.

## 7. Failure Classification

- `ENVIRONMENT_BLOCKED_CMAKE_TOO_OLD`: CMake version < 3.24
- `ENVIRONMENT_BLOCKED_TOOLCHAIN_MISSING`: CMake not found
- `CMAKE_VERSION_PRESET_AMBIGUITY_BLOCKER`: Preset version mismatch
- `CONFIGURE_SMOKE_OVERCLAIM_BLOCKER`: Configure pass claimed as build/test pass
- `BLOCKED_GATE_VALIDATION_ENVIRONMENT`: gate runner found no source failure, but a local prerequisite is blocked
- `FAIL_GATE_VALIDATION`: active gate validation found a source, structure, or policy failure
- `FAIL_BUILD_VALIDATION_STEP`: configure, build, or CTest failed

## 8. Future Phase Ownership

- **P0-Q/P0-R:** Keep Hi-Res, DSD/DoP, and mpv work in contract, research, explicit rejection, or reference-probe scope until a later implementation slice admits runtime support.
- **Mobile and broader device support:** Must add dedicated validation and release evidence rather than reusing Windows-only proof.
- **Commercial release:** Requires external signing, installer, legal, source, device-lab, and endurance evidence before approval.

## 9. Deferred Items

- compile_commands.json generation
- clang-tidy integration
- Additional CI hardening for release-lab hardware and external signing services

## 10. Stage Source

Based on V10.1 FINAL P0-B READY LOCKED PATCHED, the active execution roadmap,
and repository-controlled P0-P/P0-Q/P0-R evidence as of 2026-06-13.
