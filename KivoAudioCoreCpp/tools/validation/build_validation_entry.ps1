# =============================================================================
# build_validation_entry.ps1
# Contract: P0-D build validation entry
# =============================================================================

param(
    [string]$ProjectRoot,
    [string]$BuildDir,
    [string]$Generator,
    [string]$Configuration = "Debug"
)

$ErrorActionPreference = "Stop"

if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
}
$ProjectRoot = (Resolve-Path $ProjectRoot).Path

if (-not $BuildDir) {
    $BuildDir = Join-Path $ProjectRoot "build"
}

function Test-CommandAvailable {
    param([Parameter(Mandatory = $true)][string]$Name)
    return $null -ne (Get-Command $Name -ErrorAction SilentlyContinue)
}

function Invoke-ExternalStep {
    param(
        [Parameter(Mandatory = $true)][string]$Name,
        [Parameter(Mandatory = $true)][scriptblock]$Script
    )

    Write-Host "-----------------------------------------------------------------------------"
    Write-Host "Running: $Name"
    Write-Host "-----------------------------------------------------------------------------"
    & $Script
    $exitCode = $LASTEXITCODE
    if ($exitCode -ne 0) {
        Write-Host "CLASSIFICATION: FAIL_BUILD_VALIDATION_STEP"
        Write-Host "Failed step: $Name"
        exit $exitCode
    }
    Write-Host ""
}

Write-Host "============================================================================="
Write-Host "KivoAudioCoreCpp Build Validation Entry"
Write-Host "============================================================================="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host "BUILD_DIR: $BuildDir"
Write-Host "Date: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')"
Write-Host "============================================================================="
Write-Host ""

$hasCl = Test-CommandAvailable -Name "cl"
$hasClang = Test-CommandAvailable -Name "clang++"
$hasGcc = Test-CommandAvailable -Name "g++"
$hasNinja = Test-CommandAvailable -Name "ninja"
$hasCompiler = $hasCl -or $hasClang -or $hasGcc

if (-not $hasCompiler) {
    Write-Host "No C++ compiler found in current shell."
    Write-Host "CLASSIFICATION: BLOCKED_BY_LOCAL_TOOLCHAIN"
    exit 1
}

if (-not $Generator) {
    if ($hasNinja) {
        $Generator = "Ninja"
    } elseif ($hasCl) {
        $Generator = "Visual Studio 17 2022"
    } else {
        Write-Host "Compiler found, but no supported generator route is available."
        Write-Host "CLASSIFICATION: BLOCKED_BY_LOCAL_GENERATOR"
        exit 1
    }
}

Write-Host "Selected generator: $Generator"
Write-Host "Configuration: $Configuration"
Write-Host ""

if ($Generator -eq "Ninja") {
    Invoke-ExternalStep -Name "CMake configure (Ninja)" -Script {
        cmake -S $ProjectRoot -B $BuildDir -G Ninja
    }

    Invoke-ExternalStep -Name "CMake build (Ninja)" -Script {
        cmake --build $BuildDir --clean-first
    }

    Invoke-ExternalStep -Name "CTest (Ninja)" -Script {
        ctest --test-dir $BuildDir --output-on-failure
    }

    Write-Host "CLASSIFICATION: PASS_BUILD_VALIDATION_NINJA"
    exit 0
}

if ($Generator -eq "Visual Studio 17 2022") {
    Invoke-ExternalStep -Name "CMake configure (Visual Studio 17 2022 x64)" -Script {
        cmake -S $ProjectRoot -B $BuildDir -G "Visual Studio 17 2022" -A x64
    }

    Invoke-ExternalStep -Name "CMake build (Visual Studio)" -Script {
        cmake --build $BuildDir --config $Configuration
    }

    Invoke-ExternalStep -Name "CTest (Visual Studio)" -Script {
        ctest --test-dir $BuildDir -C $Configuration --output-on-failure
    }

    Write-Host "CLASSIFICATION: PASS_BUILD_VALIDATION_VISUAL_STUDIO"
    exit 0
}

Write-Host "Unsupported generator: $Generator"
Write-Host "CLASSIFICATION: FAIL_UNSUPPORTED_GENERATOR"
exit 1
