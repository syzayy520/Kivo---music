# P0-D-012 Windows Toolchain Locator Env Var Repair

**Task ID:** `KIVO-AUDIO-CORE-P0-D-012-WINDOWS-TOOLCHAIN-LOCATOR-ENV-VAR-REPAIR-TASK-001`

## Status

```text
REPAIR
VALIDATION_TOOLING_ONLY
NO_PRODUCTION_CODE_CHANGE
NO_RUNTIME_CHANGE
```

## Goal

Repair the Windows toolchain locator so it reads the `ProgramFiles(x86)` environment variable safely.

PowerShell cannot reliably read this variable with `$env:ProgramFiles(x86)` inside a string. The locator must use an explicit environment-variable lookup instead.

## Natural family tree

```text
KivoAudioCoreCpp/
  tools/
    validation/
      windows_toolchain_locator.ps1
  docs/
    tasks/
      p0_d_012_windows_toolchain_locator_env_var_repair_task.md
```

## Scope

Allowed files:

```text
KivoAudioCoreCpp/tools/validation/windows_toolchain_locator.ps1
KivoAudioCoreCpp/docs/tasks/p0_d_012_windows_toolchain_locator_env_var_repair_task.md
```

## Non-goals

- No production C++ edits.
- No runtime edits.
- No tests edits.
- No CMake edits.
- No gate edits.
- No installation actions.
- No machine environment mutation.

## Acceptance criteria

```text
- Locator no longer uses `$env:ProgramFiles(x86)`.
- Locator uses `[Environment]::GetEnvironmentVariable("ProgramFiles(x86)")` or equivalent safe lookup.
- Locator still reports cl/cmake/ninja status.
- Locator still reports VS toolchain found, install missing, or dev-shell required classifications.
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-D-012-WINDOWS-TOOLCHAIN-LOCATOR-ENV-VAR-REPAIR
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
