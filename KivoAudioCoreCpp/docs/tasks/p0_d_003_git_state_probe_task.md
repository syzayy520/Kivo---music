# P0-D-003 Git State Probe

**Task ID:** `KIVO-AUDIO-CORE-P0-D-003-GIT-STATE-PROBE-TASK-001`

## Status

```text
IMPLEMENTATION
VALIDATION_TOOLING_ONLY
NO_PRODUCTION_CODE_CHANGE
NO_RUNTIME_CHANGE
```

## Goal

Create a git validation probe that is safe for PowerShell and does not use unquoted upstream syntax.

Target file:

```text
tools/validation/git_state_probe.ps1
```

## Natural family tree

```text
KivoAudioCoreCpp/
  tools/
    validation/
      git_state_probe.ps1
  docs/
    tasks/
      p0_d_003_git_state_probe_task.md
```

## Scope

Allowed files:

```text
KivoAudioCoreCpp/tools/validation/git_state_probe.ps1
KivoAudioCoreCpp/docs/tasks/p0_d_003_git_state_probe_task.md
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
- Probe reports current branch.
- Probe reports HEAD.
- Probe reports origin/master.
- Probe fails if working tree is dirty.
- Probe fails if HEAD != origin/master.
- Probe does not use unquoted @{u}.
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-D-003-GIT-STATE-PROBE
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
