# KivoAudioCoreCpp Audio Core Execution Spec

**Document ID:** KIVO-CPP-AUDIO-CORE-EXECUTION-SPEC-001
**Status:** ACTIVE EXECUTION CONTROL
**Effective Date:** 2026-06-13
**Applies To:** `KivoAudioCoreCpp/`

---

## 1. Purpose

This document is the required entrypoint before making implementation,
validation, release, or architecture changes in `KivoAudioCoreCpp/`.

Every ticket must be planned as a narrow behavior slice, mapped to a natural
folder/file family, implemented with single-responsibility files, and closed by
machine-checkable guards.

## 2. Source-of-truth Precedence

When project documents conflict, use this order:

1. Safety, legal, privacy, security, licensing, and project-root constraints.
2. This execution spec for workflow, ticket shape, guard commands, and conflict
   handling.
3. `docs/architecture/audio_core_execution_roadmap.md` for phase order,
   ownership, and implementation sequence.
4. `docs/architecture/policy_index.md` for active phase status and gate set.
5. The active phase taskbook under `docs/tasks/`.
6. Current closeout or validation evidence under `docs/closeout/` and
   `docs/validation/`.
7. Older planning documents and historical notes.

If a lower-priority document appears to authorize work that a higher-priority
document forbids, the work is blocked until an explicit higher-priority
amendment records the reason, impact, and owner approval.

## 3. Non-negotiable Structure Rules

- Decide the natural family tree before adding or moving files.
- Prefer a subdirectory before adding a direct file to a broad folder.
- Keep one file responsible for one smallest coherent role.
- Split files when they start carrying multiple behaviors, policies, fixtures,
  or validation concerns.
- Pages, views, and UI surfaces compose already-owned behavior only; they must
  not carry business logic.
- Do not add empty families, speculative abstractions, or directories without
  an immediate consumer and test.
- Keep core contracts free of platform, FFmpeg, mpv/libmpv, UI, installer, and
  host-framework types unless the active architecture explicitly admits that
  boundary.

## 4. Required Execution Flow

Each ticket must follow this order:

1. Read this spec.
2. Resolve source-of-truth conflicts by the precedence above.
3. Emit the Ticket required header before modifying files.
4. Plan the natural family tree.
5. Implement the smallest behavior slice.
6. Run static structure self-checks.
7. Accumulate related local checks.
8. Run the required machine-checkable guard commands.
9. Report using the Ticket acceptance form.

Do not report a pass when a source failure, missing local prerequisite, or
external blocker is present. Classify it honestly.

## 5. Ticket Required Header

Before file edits, report this header:

```text
Ticket ID:
Behavior:
Source-of-truth basis:
Non-goals:
Natural family tree:
Implementation boundaries:
Tests/evidence to add or update:
Machine-checkable guards to run:
```

The header must be specific enough that a reviewer can reject scope creep
before implementation begins.

## 6. Ticket Acceptance Form

After implementation and validation, report this form:

```text
Ticket ID:
Behavior completed:
Files/families changed:
Tests/evidence:
Machine-checkable guards:
Source-of-truth conflicts resolved:
Known blockers:
Commercial/runtime claim impact:
Final classification:
```

`Final classification` must be one of:

- `PASS_LOCAL_ENGINEERING_GUARDS`
- `BLOCKED_BY_LOCAL_ENVIRONMENT`
- `BLOCKED_BY_EXTERNAL_EVIDENCE`
- `FAIL_SOURCE_OR_ARCHITECTURE`

## 7. Machine-checkable Guard Commands

For normal implementation slices, run and report:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\gates\run_cpp_audio_core_gates.ps1 -ProjectRoot .
powershell -ExecutionPolicy Bypass -File .\tools\validation\build_validation_entry.ps1 -ProjectRoot . -BuildDir build -Generator Ninja -Configuration Debug
$repo = (Resolve-Path ..).Path; git -c safe.directory="$repo" -C "$repo" diff --check -- KivoAudioCoreCpp
```

For documentation-only workflow changes, also prove that this spec contains the
required process anchors:

```powershell
rg -n "Source-of-truth Precedence|Ticket Required Header|Ticket Acceptance Form|Machine-checkable Guard Commands" docs\AUDIO_CORE_EXECUTION_SPEC.md
```

For release-candidate or commercial-claim changes, the release policy may
require additional commands such as release-candidate validation, binary
reproducibility, SBOM, notices, signing, package, privacy, and device-matrix
checks.

## 8. Claim Control

Docs, tests, and probes may reserve future behavior, but they must not create a
product claim.

Current P0-Q/P0-R rules remain:

- Hi-Res PCM contracts and tests may exist.
- Native DSD and DoP value contracts may exist.
- Unsupported DSD/DoP decode classification may exist.
- mpv reference probing may exist.
- DSD/DoP playback, passthrough, mpv compatibility mode, and commercial support
  claims remain unauthorized until a later implementation slice admits and
  proves them.
