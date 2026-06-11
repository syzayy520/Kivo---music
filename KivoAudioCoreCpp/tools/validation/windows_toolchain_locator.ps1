# =============================================================================
# windows_toolchain_locator.ps1
# Contract: P0-D Windows toolchain locator
# =============================================================================

param(
    [string]$ProjectRoot
)

$ErrorActionPreference = "Continue"

if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
}
$ProjectRoot = (Resolve-Path $ProjectRoot).Path

. (Join-Path $PSScriptRoot "import_msvc_environment.ps1")
$msvcEnvironmentImported = Import-KivoMsvcEnvironment

function Test-CommandAvailable {
    param([Parameter(Mandatory = $true)][string]$Name)
    return $null -ne (Get-Command $Name -ErrorAction SilentlyContinue)
}

function Add-VswhereCandidate {
    param(
        [System.Collections.Generic.List[string]]$Candidates,
        [string]$BasePath
    )

    if ($BasePath) {
        $Candidates.Add((Join-Path $BasePath "Microsoft Visual Studio\Installer\vswhere.exe"))
    }
}

Write-Host "============================================================================="
Write-Host "KivoAudioCoreCpp Windows Toolchain Locator"
Write-Host "============================================================================="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host "Date: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')"
Write-Host "============================================================================="
Write-Host ""

$hasCl = Test-CommandAvailable -Name "cl"
$hasCmake = Test-CommandAvailable -Name "cmake"
$hasNinja = Test-CommandAvailable -Name "ninja"

Write-Host ("  {0,-12} {1}" -f "cl", $(if ($hasCl) { "FOUND" } else { "MISSING" }))
Write-Host ("  {0,-12} {1}" -f "cmake", $(if ($hasCmake) { "FOUND" } else { "MISSING" }))
Write-Host ("  {0,-12} {1}" -f "ninja", $(if ($hasNinja) { "FOUND" } else { "MISSING" }))
Write-Host ""

if ($hasCl) {
    Write-Host "MSVC cl is available in this validation process."
    Write-Host ("Environment imported automatically: {0}" -f $msvcEnvironmentImported)
    Write-Host "CLASSIFICATION: PASS_TOOLCHAIN_READY"
    exit 0
}

$vswhereCommand = Get-Command "vswhere.exe" -ErrorAction SilentlyContinue
$vswhereCandidates = [System.Collections.Generic.List[string]]::new()

if ($vswhereCommand) {
    $vswhereCandidates.Add($vswhereCommand.Source)
}

$programFilesX86 = [Environment]::GetEnvironmentVariable("ProgramFiles(x86)")
$programFiles = [Environment]::GetEnvironmentVariable("ProgramFiles")

Add-VswhereCandidate -Candidates $vswhereCandidates -BasePath $programFilesX86
Add-VswhereCandidate -Candidates $vswhereCandidates -BasePath $programFiles

$vswhere = $vswhereCandidates | Where-Object { $_ -and (Test-Path $_) } | Select-Object -First 1

if ($vswhere) {
    Write-Host "vswhere found: $vswhere"
    $installations = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath 2>$null
    if ($installations) {
        Write-Host "Visual Studio C++ toolchain installation found:"
        $installations | ForEach-Object { Write-Host "  $_" }
        Write-Host ""
        Write-Host "Open one of these shells before running CMake build validation:"
        Write-Host "  x64 Native Tools Command Prompt for VS"
        Write-Host "  Developer PowerShell for VS"
        Write-Host "CLASSIFICATION: PASS_VS_TOOLCHAIN_FOUND_REQUIRES_DEV_SHELL"
        exit 0
    }

    Write-Host "vswhere is present, but no C++ workload installation was found."
    Write-Host "CLASSIFICATION: BLOCKED_BY_LOCAL_TOOLCHAIN_INSTALL_NOT_FOUND"
    exit 1
}

Write-Host "vswhere.exe was not found in standard Visual Studio installer locations."
Write-Host "Install Visual Studio Build Tools with Desktop development with C++."
Write-Host "CLASSIFICATION: BLOCKED_BY_LOCAL_TOOLCHAIN_INSTALL_NOT_FOUND"
exit 1
