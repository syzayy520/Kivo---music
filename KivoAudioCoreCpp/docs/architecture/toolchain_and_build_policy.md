# Toolchain and Build Policy

**Purpose:** Define the C++ toolchain, build system, and compilation standards for KivoAudioCoreCpp.  
**Scope:** All phases from P0-B onward.  
**Current Phase Rule:** P0-B is configure-only. No build targets exist.  
**Last Updated:** P0-B (V10.1)  

## 1. Purpose

This policy governs the C++ toolchain selection, build system configuration, and compilation standards. It ensures reproducible, auditable builds across development machines.

## 2. Scope

Applies to all CMakeLists.txt, CMakePresets.json, and any future build configuration files within KivoAudioCoreCpp/.

## 3. Current Phase Rule (P0-B)

P0-B is configure-only. The CMakeLists.txt defines only `cmake_minimum_required`, `project`, and standard settings. There are no build targets, test targets, or runtime targets.

## 4. Allowed

- cmake_minimum_required(VERSION 3.24)
- project(KivoAudioCoreCpp VERSION 0.0.0 LANGUAGES CXX)
- set(CMAKE_CXX_STANDARD 20)
- set(CMAKE_CXX_STANDARD_REQUIRED ON)
- set(CMAKE_CXX_EXTENSIONS OFF)
- CMakePresets.json with version 5 and configure-only preset
- Comments documenting future MSVC/Windows x64 policy

## 5. Forbidden

- add_executable (P0-B)
- add_library (P0-B)
- target_sources (P0-B)
- target_link_libraries (P0-B)
- find_package for third-party dependencies (P0-B)
- FetchContent (all phases)
- install/package commands (P0-B)
- vcpkg/conan/toolchain file references (P0-B)
- Hardcoded absolute paths in CMakePresets
- Architecture pinning (unless Visual Studio generator explicitly needed)

## 6. Gate / Check Method

- `check_toolchain_configure_gate.ps1` validates CMake availability and runs configure-only smoke
- `check_forbidden_token_gate.ps1` validates no implementation tokens in CMakeLists.txt

## 7. Failure Classification

- `ENVIRONMENT_BLOCKED_CMAKE_TOO_OLD`: CMake version < 3.24
- `ENVIRONMENT_BLOCKED_TOOLCHAIN_MISSING`: CMake not found
- `CMAKE_VERSION_PRESET_AMBIGUITY_BLOCKER`: Preset version mismatch
- `CONFIGURE_SMOKE_OVERCLAIM_BLOCKER`: Configure pass claimed as build/test pass
- `CMAKE_CONFIGURE_ONLY_SCOPE_BLOCKER`: Build targets found in P0-B CMakeLists.txt

## 8. Future Phase Ownership

- **P0-C:** Extend to contract build/test harness
- **P0-H:** Add playback target
- **P0-P:** Add installer/packaging targets

## 9. Deferred Items

- MSVC warning level (/W4) enforcement
- Windows x64 target architecture enforcement
- _WIN32_WINNT >= 0x0A00 enforcement
- compile_commands.json generation
- clang-tidy integration

## 10. Stage Source

Based on V10.1 FINAL P0-B READY LOCKED PATCHED.
