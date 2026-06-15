# Dependency and License Policy

**Purpose:** Govern all third-party dependencies, their licenses, and integration methods for KivoAudioCoreCpp.  
**Scope:** All phases from P0-B onward.  
**Current Phase Rule:** P0-P packages only the approved dynamically linked
FFmpeg baseline and records its provenance, hashes, notices, and SBOM.
**Last Updated:** 2026-06-12

## 1. Purpose

This policy ensures all third-party dependencies are legally auditable, version-pinned, and reproducible. It prevents uncontrolled dependency injection and ensures commercial release compliance.

## 2. Scope

Applies to all C++ source files, build configuration, and any future dependency management within KivoAudioCoreCpp/.

## 3. Current Phase Rule (P0-P)

Release builds use the documented FFmpeg n7.1 shared-library baseline. The
build performs no network download. Commercial approval requires exact
corresponding-source custody and legal review in addition to repository
records.

## 4. Allowed

- Standard C++20 library (compiler-provided)
- Windows SDK (system-provided, documented in toolchain policy)
- Approved FFmpeg shared libraries from the documented baseline
- Windows SDK and MSVC runtime
- Generated third-party notices and CycloneDX SBOM

## 5. Forbidden

- Unapproved vcpkg or Conan dependency injection
- FetchContent declarations (all phases)
- git submodule for third-party (P0-B)
- Downloading dependencies during build (all phases)
- Checked-in vendor/, third_party/, bin/, or lib/ dependency payloads
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

- **P0-P:** Release provenance, notices, SBOM, and source-retention record
- **P0-Q:** Separate legal and technical review for new format dependencies
- **P0-R:** Separate mpv/libmpv license and distribution audit

## 9. Deferred Items

- Exact FFmpeg corresponding-source archive custody
- External commercial legal approval
- vcpkg/Conan evaluation for dependency management (post P0-I)

## 10. Stage Source

Based on V10.1 FINAL P0-B READY LOCKED PATCHED.

**Iron Rule 22:** FFmpeg license is not a reminder — it is a commercial release gate.
