# Validation and Release Policy

**Purpose:** Define validation gates, release criteria, and quality assurance requirements for each phase.  
**Scope:** All phases from P0-B onward.  
**Current Phase Rule:** P0-P requires full build, test, repository, binary,
package, compliance, privacy, and release-matrix evidence.
**Last Updated:** 2026-06-12

## 1. Purpose

This policy governs how each phase is validated before proceeding to the next. It ensures no phase can advance without meeting its gates, and prevents quality debt from accumulating.

## 2. Scope

Applies to all phase transitions, gate scripts, and release criteria within KivoAudioCoreCpp/.

## 3. Current Phase Rule (P0-P)

P0-P validation builds twice from clean Release directories, runs full CTest
and repository gates, verifies binary identity, exports, PE hardening, version
resources, package hashes, symbol separation, privacy, notices, SBOM, and
signature status. Missing external evidence blocks commercial approval.

## 4. Allowed

- Full gates for every release candidate
- Phase-specific gate criteria
- Environment classification (blocked/available)
- Honest failure reporting
- Explicit engineering-candidate classification when external gates are open

## 5. Forbidden

- Claiming configure smoke as build/test proof
- Skipping gates for phase transitions
- Auto-committing or auto-pushing from gate scripts
- Destructive gate operations
- Network access from gate scripts
- Admin-requiring gate scripts
- False PASS when environment is blocked
- Commercial approval while signing, legal, source, installer, or laboratory
  evidence is open

## 6. Gate / Check Method

- `run_cpp_audio_core_gates.ps1` aggregates repository gates
- `run_release_candidate.ps1` aggregates release build and package evidence
- Each gate script has specific scope and output format
- Gate results must be copy-pasteable into closeout

## 7. Failure Classification

- `NO_GO_GATE_SCRIPT_CONTRACT_VIOLATION`: Gate script violates contract
- `CONFIGURE_SMOKE_OVERCLAIM_BLOCKER`: Smoke claimed as full validation
- `GATE_SKIP_BLOCKER`: Gate skipped without justification
- `ENVIRONMENT_BLOCKED_*`: Toolchain/environment issues

## 8. Future Phase Ownership

- **P0-C:** Contract test gates
- **P0-D:** Fake backend validation gates
- **P0-H:** Real WASAPI output validation
- **P0-I:** Full decode-output loop validation
- **P0-P:** Commercial release validation matrix

## 9. Deferred Items

- CI signing service integration
- Product installer integration
- External device-lab automation

## 10. Go / No-Go Decision

| Condition | Classification |
|-----------|---------------|
| Engineering gates pass, external gates open | ENGINEERING_RC_READY_COMMERCIAL_RELEASE_BLOCKED |
| Engineering and external gates pass | COMMERCIAL_RELEASE_APPROVED |
| Build, test, gate, reproducibility, or package failure | RELEASE_ENGINEERING_FAILED |

**Iron Rule 51:** Environment must be classified honestly; cannot fabricate PASS.
