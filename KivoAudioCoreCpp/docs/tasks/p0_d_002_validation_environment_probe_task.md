# P0-D-002 Validation Environment Probe

**Task ID:** `KIVO-AUDIO-CORE-P0-D-002-VALIDATION-ENVIRONMENT-PROBE-TASK-001`

## Status

```text
IMPLEMENTATION
VALIDATION_TOOLING_ONLY
NO_PRODUCTION_CODE_CHANGE
NO_RUNTIME_CHANGE
```

## Goal

Create the first P0-D validation tooling file:

```text
tools/validation/environment_probe.ps1
```

The probe must report whether the current shell exposes the required validation tools for the C++ audio core.

## Natural family tree

```text
KivoAudioCoreCpp/
  tools/
    validation/
      environment_probe.ps1
  docs/
    tasks/
      p0_d_002_validation_environment_probe_task.md
```

## Scope

Allowed files:

```text
KivoAudioCoreCpp/tools/validation/environment_probe.ps1
KivoAudioCoreCpp/docs/tasks/p0_d_002_validation_environment_probe_task.md
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
- Probe discovers git, cmake, cl, clang++, g++, and ninja availability.
- Probe does not mutate files.
- Probe emits a clear PASS or BLOCKED classification.
- Probe can be run from ordinary PowerShell or Developer PowerShell.
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-D-002-VALIDATION-ENVIRONMENT-PROBE
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
