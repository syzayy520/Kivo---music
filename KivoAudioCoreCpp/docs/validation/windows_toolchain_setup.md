# Windows Toolchain Setup for KivoAudioCoreCpp

## Required tooling

Install Visual Studio 2022 or Visual Studio Build Tools 2022 with:

```text
Desktop development with C++
MSVC v143 build tools
Windows 10/11 SDK
CMake tools for Windows
Ninja build system
```

## Correct shell

Do not run full C++ build validation from ordinary PowerShell unless the compiler is already available.

Preferred shells:

```text
x64 Native Tools Command Prompt for VS
Developer PowerShell for VS
```

These shells initialize MSVC environment variables so `cl` can be discovered by CMake.

## First checks after install

From the repository root:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\validation\git_state_probe.ps1
powershell -ExecutionPolicy Bypass -File .\tools\validation\environment_probe.ps1
powershell -ExecutionPolicy Bypass -File .\tools\validation\windows_toolchain_locator.ps1
```

Expected ready states:

```text
PASS_GIT_STATE_CLEAN_HEAD_EQUALS_REMOTE
PASS_TOOLCHAIN_PROBE_READY_NINJA
PASS_TOOLCHAIN_SHELL_READY
```

If Visual Studio is installed but the current shell is wrong, the locator may report:

```text
PASS_VS_TOOLCHAIN_FOUND_REQUIRES_DEV_SHELL
```

Open a Developer shell and rerun the probes.

## Configure paths

Preferred Ninja route:

```powershell
cmake -S . -B build -G Ninja
cmake --build build --clean-first
ctest --test-dir build --output-on-failure
```

Fallback Visual Studio route:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

## Validation scripts

Once the shell is ready, use:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\validation\build_validation_entry.ps1
powershell -ExecutionPolicy Bypass -File .\tools\validation\gate_validation_entry.ps1
powershell -ExecutionPolicy Bypass -File .\tools\validation\validation_batch_entry.ps1
```

For a light check that does not build:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\validation\validation_smoke_entry.ps1
```

## Known PowerShell trap

Do not use this unquoted command in shared instructions:

```powershell
git rev-parse @{u}
```

Use one of:

```powershell
git rev-parse origin/master
git rev-parse '@{u}'
```
