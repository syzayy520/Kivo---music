# Dependency and License Policy

**Purpose:** Govern all third-party dependencies, their licenses, and integration methods for KivoAudioCoreCpp.  
**Scope:** All phases from P0-B onward.  
**Current Phase Rule:** P0-B has zero third-party dependencies.  
**Last Updated:** P0-B (V10.1)  

## 1. Purpose

This policy ensures all third-party dependencies are legally auditable, version-pinned, and reproducible. It prevents uncontrolled dependency injection and ensures commercial release compliance.

## 2. Scope

Applies to all C++ source files, build configuration, and any future dependency management within KivoAudioCoreCpp/.

## 3. Current Phase Rule (P0-B)

P0-B has ZERO third-party dependencies. No FetchContent, no vcpkg, no Conan, no vendor/, no third_party/, no bin/, no lib/.

## 4. Allowed

- Standard C++20 library (compiler-provided)
- Windows SDK (system-provided, documented in toolchain policy)
- Documentation about future FFmpeg integration (docs only)
- Documentation about future WASAPI usage (docs only)

## 5. Forbidden

- vcpkg manifest or configuration (P0-B)
- Conan configuration (P0-B)
- FetchContent declarations (all phases)
- git submodule for third-party (P0-B)
- Downloading dependencies during build (all phases)
- vendor/, third_party/, bin/, lib/ directories (P0-B)
- Undocumented DLL dependencies (all phases)
- Dependencies with incompatible licenses for commercial distribution

## 6. Gate / Check Method

- `check_dependency_license_gate.ps1` scans for dependency indicators
- No network access is permitted during any gate execution

## 7. Failure Classification

- `P0B_NETWORK_OR_DEPENDENCY_BOOTSTRAP_BLOCKER`: Network dependency download attempted
- `DEPENDENCY_LICENSE_AUDIT_FAIL`: Dependency with incompatible license detected
- `UNDOCUMENTED_DLL_BLOCKER`: DLL dependency without documentation

## 8. Future Phase Ownership

- **P0-G:** FFmpeg dependency integration with license compliance
- **P0-H:** WASAPI integration (Windows SDK, no additional license)
- **P0-O:** Tauri integration boundary (host ABI)

## 9. Deferred Items

- FFmpeg license audit (LGPL/GPL implications for commercial distribution)
- FFmpeg DLL packaging and distribution strategy
- vcpkg/Conan evaluation for dependency management (post P0-I)

## 10. Stage Source

Based on V10.1 FINAL P0-B READY LOCKED PATCHED.

**Iron Rule 22:** FFmpeg license is not a reminder — it is a commercial release gate.
