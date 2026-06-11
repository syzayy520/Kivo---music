# P0-D-009 Windows Toolchain Locator

**Task ID:** `KIVO-AUDIO-CORE-P0-D-009-WINDOWS-TOOLCHAIN-LOCATOR-TASK-001`

## Status

```text
IMPLEMENTATION
VALIDATION_TOOLING_ONLY
NO_PRODUCTION_CODE_CHANGE
NO_RUNTIME_CHANGE
```

## Goal

Create a Windows toolchain locator that helps identify whether the current shell is already a C++ build shell, or whether Visual Studio / Build Tools exists but the developer needs to open a Developer shell.

Target file:

```text
tools/validation/windows_toolchain_locator.ps1
```

## Natural family tree

```text
KivoAudioCoreCpp/
  tools/
    validation/
      windows_toolchain_locator.ps1
  docs/
    tasks/
      p0_d_009_windows_toolchain_locator_task.md
```

## Scope

Allowed files:

```text
KivoAudioCoreCpp/tools/validation/windows_toolchain_locator.ps1
KivoAudioCoreCpp/docs/tasks/p0_d_009_windows_toolchain_locator_task.md
```

## Non-goals

- No production C++ edits.
- No runtime edits.
- No tests edits.
- No CMake edits.
- No gate edits.
- No installation actions.
- No environment mutation.

## Acceptance criteria

```text
- Script detects cl availability in current shell.
- Script searches for vswhere.exe in standard Visual Studio installer location.
- Script reports Visual Studio installation paths when discoverable.
- Script emits a PASS/BLOCKED classification.
- Script gives next-step shell guidance without modifying the machine.
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-D-009-WINDOWS-TOOLCHAIN-LOCATOR
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
