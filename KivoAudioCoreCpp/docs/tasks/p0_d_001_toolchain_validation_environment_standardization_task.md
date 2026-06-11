# P0-D-001 Toolchain / Validation Environment Standardization

**Task ID:** `KIVO-AUDIO-CORE-P0-D-001-TOOLCHAIN-VALIDATION-ENVIRONMENT-STANDARDIZATION-TASK-001`

## Status

```text
PLANNING_FIRST
DOCUMENTATION_ONLY_BASELINE
NO_PRODUCTION_CODE_CHANGE
NO_RUNTIME_CHANGE
```

## Why this task exists

P0-C reached closeout decision state after the P0-C-043 closeout ledger. The next blocker is not contract-code correctness; it is reproducible local validation.

The latest attempted local verification exposed these environment issues:

```text
No CMAKE_CXX_COMPILER could be found
Ninja was not available in the ordinary PowerShell environment
PowerShell @{u} can enter continuation mode unless quoted or replaced with origin/master
```

P0-D begins by standardizing how developers prove the C++ audio core is being validated from a real toolchain environment before runtime work resumes.

## Goal

Define the P0-D validation environment baseline for Windows desktop C++ audio-core development.

This task does not implement runtime code. It records the required toolchain, shell, generators, git probes, and validation sequence that later P0-D implementation tasks may convert into scripts.

## Non-goals

- No production C++ headers or sources.
- No runtime implementation.
- No WASAPI, FFmpeg, device, output, decoder, thread, buffer, or scheduler work.
- No CMake target edits.
- No test changes.
- No gate script changes.
- No new tool script in this task.
- No CI changes.

## Natural family tree

This task belongs to the documentation task family only:

```text
KivoAudioCoreCpp/
  docs/
    tasks/
      p0_d_001_toolchain_validation_environment_standardization_task.md
```

Planned future tool family, not created in this task:

```text
KivoAudioCoreCpp/
  tools/
    validation/
      environment_probe.ps1
      build_validation_entry.ps1
      gate_validation_entry.ps1
```

The future `tools/validation/` family, if created, must remain focused on validation orchestration only. It must not contain audio runtime logic or platform abstractions.

## Required Windows toolchain baseline

At least one valid Windows C++ toolchain route must be available.

### Preferred route: MSVC + Ninja

Run from one of:

```text
x64 Native Tools Command Prompt for VS
Developer PowerShell for VS
```

Required commands must resolve:

```powershell
where.exe cl
where.exe ninja
cmake --version
git --version
```

Expected configure path:

```powershell
cmake -S . -B build -G Ninja
cmake --build build --clean-first
ctest --test-dir build --output-on-failure
```

### Fallback route: Visual Studio generator

If Ninja is not available but MSVC is available:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

## Git probe rules

PowerShell must not use unquoted `@{u}` in reusable validation commands.

Allowed:

```powershell
git rev-parse HEAD
git rev-parse origin/master
git rev-parse '@{u}'
```

Forbidden in shared task instructions:

```powershell
git rev-parse @{u}
```

Reason: ordinary PowerShell may interpret `@{}` as hashtable syntax and enter continuation mode.

## Required validation sequence

Once the toolchain is available, P0-D validation entry points must support this sequence:

```text
1. git status --short
2. git branch --show-current
3. git rev-parse HEAD
4. git rev-parse origin/master
5. CMake configure
6. CMake build
7. CTest
8. contract_tests executable
9. capability_tests executable
10. aggregate gates 9/9
11. forbidden token scan
12. cross-family include scan
13. git diff --check
14. final git status --short
```

## Validation classification vocabulary

Use these classifications consistently:

```text
PASS_VERIFIED_BY_USER_LOCAL_RUN
BLOCKED_BY_LOCAL_TOOLCHAIN
BLOCKED_BY_GATE_DETAIL_REQUIRED
STATIC_SELF_CHECK_PASS_FULL_VALIDATION_DEFERRED
IMPLEMENTED_PUSHED_LOCAL_HEAD_EQUALS_REMOTE_HEAD
```

## STOP conditions for future implementation tasks

Future P0-D implementation tasks must STOP if:

```text
working tree is not clean
git HEAD != origin/master before starting
C++ compiler is not discoverable
selected generator cannot configure
validation script would modify production code
validation script would hide failed commands
validation script cannot report failed gate names
PowerShell command uses unquoted @{u}
```

## Acceptance criteria for this documentation baseline

```text
- Document is stored under docs/tasks.
- No production file is touched.
- No test file is touched.
- No CMake file is touched.
- No gate script is touched.
- P0-D validation vocabulary is defined.
- P0-D Windows toolchain baseline is defined.
- Future validation tool family is planned but not created.
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-D-001-TOOLCHAIN-VALIDATION-ENVIRONMENT-STANDARDIZATION
DOCUMENTATION_ONLY_BASELINE
NO_PRODUCTION_CODE_CHANGE
READY_FOR_P0_D_VALIDATION_TOOLING_IMPLEMENTATION
```
