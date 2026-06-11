# P0-D Validation Tooling Usage

## Purpose

P0-D standardizes how KivoAudioCoreCpp validation is run from a reproducible Windows C++ environment.

This guide covers the first two probes:

```text
tools/validation/environment_probe.ps1
tools/validation/git_state_probe.ps1
```

## Shell requirement

For build and test validation, prefer one of:

```text
x64 Native Tools Command Prompt for VS
Developer PowerShell for VS
```

Ordinary PowerShell may not expose `cl`, `ninja`, or the required compiler environment.

## Environment probe

Run from the repository root:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\validation\environment_probe.ps1
```

Expected success classifications:

```text
PASS_TOOLCHAIN_PROBE_READY_NINJA
PASS_TOOLCHAIN_PROBE_READY_COMPILER_NO_NINJA
```

If the compiler or required base tools are missing:

```text
BLOCKED_BY_LOCAL_TOOLCHAIN
```

## Git state probe

Run from the repository root:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\validation\git_state_probe.ps1
```

Expected success classification:

```text
PASS_GIT_STATE_CLEAN_HEAD_EQUALS_REMOTE
```

## Safe git commands

Use these in PowerShell:

```powershell
git status --short
git branch --show-current
git rev-parse HEAD
git rev-parse origin/master
git rev-parse '@{u}'
```

Do not use this unquoted form in shared PowerShell instructions:

```powershell
git rev-parse @{u}
```

It can enter continuation mode because PowerShell treats `@{}` as hashtable syntax.

## Validation workflow baseline

Before running full build/test/gates, first run:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\validation\git_state_probe.ps1
powershell -ExecutionPolicy Bypass -File .\tools\validation\environment_probe.ps1
```

If both are ready, continue to configure/build/test using either Ninja or the Visual Studio generator according to the available tools.
