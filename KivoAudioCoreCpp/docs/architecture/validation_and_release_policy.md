# Validation and Release Policy

**Purpose:** Define validation gates, release criteria, and quality assurance requirements for each phase.  
**Scope:** All phases from P0-B onward.  
**Current Phase Rule:** P0-B uses smoke gates for structural validation.  
**Last Updated:** P0-B (V10.1)  

## 1. Purpose

This policy governs how each phase is validated before proceeding to the next. It ensures no phase can advance without meeting its gates, and prevents quality debt from accumulating.

## 2. Scope

Applies to all phase transitions, gate scripts, and release criteria within KivoAudioCoreCpp/.

## 3. Current Phase Rule (P0-B)

P0-B validation uses smoke gates:
- Forbidden token gate (smoke)
- Genealogy gate (smoke)
- No empty forest gate (smoke)
- No audio runtime gate (smoke)
- Policy substance gate (smoke)
- Dependency/license gate (smoke)
- Toolchain/configure gate (smoke)

Configure-only smoke PASS does NOT mean build/test/runtime PASS.

## 4. Allowed

- Smoke-level gates for P0-B
- Full gates for P0-C+ phases
- Phase-specific gate criteria
- Environment classification (blocked/available)
- Honest failure reporting

## 5. Forbidden

- Claiming configure smoke as build/test proof
- Skipping gates for phase transitions
- Auto-committing or auto-pushing from gate scripts
- Destructive gate operations
- Network access from gate scripts
- Admin-requiring gate scripts
- False PASS when environment is blocked

## 6. Gate / Check Method

- `run_cpp_audio_core_gates.ps1` aggregates all gate results
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

- CI/CD pipeline integration
- Automated regression testing
- Performance benchmarking gates
- Device compatibility matrix validation

## 10. Go / No-Go Decision

| Condition | Classification |
|-----------|---------------|
| All gates PASS, git clean, push clear | GO_TO_P0_C |
| Toolchain blocked but docs/policy complete | DOCS_AND_GATES_COMPLETE_BUT_NO_GO_TO_P0_C_UNTIL_TOOLCHAIN_CONFIRMED |
| Scope violation | NO_GO_SCOPE_VIOLATION |
| Policy placeholder only | NO_GO_POLICY_PLACEHOLDER |
| Dirty working tree | NO_GO_DIRTY_TREE |
| Runtime/platform pollution | NO_GO_RUNTIME_OR_PLATFORM_POLLUTION |

**Iron Rule 51:** Environment must be classified honestly; cannot fabricate PASS.
