# P0-D-004 Validation Tooling Usage Guide

**Task ID:** `KIVO-AUDIO-CORE-P0-D-004-VALIDATION-TOOLING-USAGE-GUIDE-TASK-001`

## Status

```text
DOCUMENTATION
VALIDATION_TOOLING_GUIDE
NO_PRODUCTION_CODE_CHANGE
NO_RUNTIME_CHANGE
```

## Goal

Create a concise usage guide for the P0-D validation probes introduced in this batch.

Target file:

```text
docs/validation/p0_d_validation_tooling_usage.md
```

## Natural family tree

```text
KivoAudioCoreCpp/
  docs/
    validation/
      p0_d_validation_tooling_usage.md
    tasks/
      p0_d_004_validation_tooling_usage_guide_task.md
```

## Scope

Allowed files:

```text
KivoAudioCoreCpp/docs/validation/p0_d_validation_tooling_usage.md
KivoAudioCoreCpp/docs/tasks/p0_d_004_validation_tooling_usage_guide_task.md
```

## Non-goals

- No production C++ edits.
- No runtime edits.
- No test edits.
- No CMake edits.
- No gate edits.
- No CI edits.

## Acceptance criteria

```text
- Guide explains ordinary PowerShell limitations.
- Guide explains Developer PowerShell / x64 Native Tools requirement.
- Guide gives safe git probe commands.
- Guide gives environment probe and git probe usage.
- Guide avoids unquoted @{u} commands.
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-D-004-VALIDATION-TOOLING-USAGE-GUIDE
DOCUMENTATION_ONLY
READY_FOR_LOCAL_DOCUMENT_REVIEW
```
