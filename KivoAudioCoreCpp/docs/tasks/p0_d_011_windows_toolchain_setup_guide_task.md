# P0-D-011 Windows Toolchain Setup Guide

**Task ID:** `KIVO-AUDIO-CORE-P0-D-011-WINDOWS-TOOLCHAIN-SETUP-GUIDE-TASK-001`

## Status

```text
DOCUMENTATION
VALIDATION_TOOLING_GUIDE
NO_PRODUCTION_CODE_CHANGE
NO_RUNTIME_CHANGE
```

## Goal

Create a Windows setup guide for the validation toolchain required by KivoAudioCoreCpp.

Target file:

```text
docs/validation/windows_toolchain_setup.md
```

## Natural family tree

```text
KivoAudioCoreCpp/
  docs/
    validation/
      windows_toolchain_setup.md
    tasks/
      p0_d_011_windows_toolchain_setup_guide_task.md
```

## Scope

Allowed files:

```text
KivoAudioCoreCpp/docs/validation/windows_toolchain_setup.md
KivoAudioCoreCpp/docs/tasks/p0_d_011_windows_toolchain_setup_guide_task.md
```

## Non-goals

- No production C++ edits.
- No runtime edits.
- No tests edits.
- No CMake edits.
- No gate edits.
- No install automation.

## Acceptance criteria

```text
- Guide names required Visual Studio Build Tools workload.
- Guide names required components.
- Guide explains ordinary PowerShell vs Developer shell.
- Guide explains Ninja and Visual Studio generator paths.
- Guide gives first probe commands after install.
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-D-011-WINDOWS-TOOLCHAIN-SETUP-GUIDE
DOCUMENTATION_ONLY
READY_FOR_LOCAL_DOCUMENT_REVIEW
```
