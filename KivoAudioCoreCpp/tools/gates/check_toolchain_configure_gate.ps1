# =============================================================================
# check_toolchain_configure_gate.ps1
# Toolchain / Configure Gate — CMake Availability and Configure Smoke
# =============================================================================
# Validates:
#   1. CMake is available
#   2. CMake version >= 3.24
#   3. Configure-only smoke passes (no build)
# =============================================================================

param(
    [string]$ProjectRoot
)

$ErrorActionPreference = "Stop"

# Resolve PROJECT_ROOT
if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
    $ProjectRoot = Resolve-Path $ProjectRoot
}

Write-Host "=== Toolchain / Configure Gate ==="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host ""

# --- 1. Check CMake availability ---
$cmakePath = Get-Command cmake -ErrorAction SilentlyContinue
if (-not $cmakePath) {
    Write-Host "ENVIRONMENT_BLOCKED_TOOLCHAIN_MISSING"
    Write-Host "CMake is not found in PATH."
    Write-Host ""
    Write-Host "Classification: DOCS_AND_GATES_COMPLETE_BUT_NO_GO_TO_P0_C_UNTIL_TOOLCHAIN_CONFIRMED"
    exit 0  # Not a gate failure — environment issue
}

# --- 2. Check CMake version ---
$cmakeVersionOutput = & cmake --version 2>&1 | Select-Object -First 1
if ($cmakeVersionOutput -match "cmake version (\d+)\.(\d+)") {
    $major = [int]$Matches[1]
    $minor = [int]$Matches[2]
    Write-Host "CMake Version: $major.$minor"
    
    if ($major -lt 3 -or ($major -eq 3 -and $minor -lt 24)) {
        Write-Host ""
        Write-Host "ENVIRONMENT_BLOCKED_CMAKE_TOO_OLD"
        Write-Host "CMake version $major.$minor found. Minimum required: 3.24"
        Write-Host ""
        Write-Host "Classification: DOCS_AND_GATES_COMPLETE_BUT_NO_GO_TO_P0_C_UNTIL_TOOLCHAIN_CONFIRMED"
        exit 0  # Not a gate failure — environment issue
    }
} else {
    Write-Host "WARNING: Could not parse CMake version from: $cmakeVersionOutput"
}

# --- 3. Run configure-only smoke ---
Write-Host ""
Write-Host "Running configure-only smoke..."
Write-Host "Preset: p0b-configure-smoke"
Write-Host ""

$presetPath = Join-Path $ProjectRoot "CMakePresets.json"
if (-not (Test-Path $presetPath)) {
    Write-Host "FAIL"
    Write-Host "CMakePresets.json not found."
    exit 1
}

$buildDir = Join-Path $ProjectRoot ".build\p0b-configure-smoke"

# Clean previous configure output
if (Test-Path $buildDir) {
    Remove-Item -Path $buildDir -Recurse -Force -ErrorAction SilentlyContinue
}

try {
    Push-Location $ProjectRoot
    $configureOutput = & cmake --preset p0b-configure-smoke 2>&1
    $configureExitCode = $LASTEXITCODE
    Pop-Location
} catch {
    Pop-Location
    Write-Host "FAIL"
    Write-Host "Configure smoke failed with exception: $_"
    exit 1
}

if ($configureExitCode -ne 0) {
    Write-Host "FAIL"
    Write-Host "Configure smoke failed with exit code: $configureExitCode"
    Write-Host ""
    Write-Host "Output:"
    $configureOutput | ForEach-Object { Write-Host "  $_" }
    exit 1
}

Write-Host "PASS_CONFIGURE_SMOKE"
Write-Host ""
Write-Host "CMake configure-only smoke passed."
Write-Host ""
Write-Host "IMPORTANT:"
Write-Host "  This is a CONFIGURE_SMOKE_ONLY result."
Write-Host "  This is NOT a BUILD pass."
Write-Host "  This is NOT a TEST pass."
Write-Host "  This is NOT a RUNTIME pass."
Write-Host "  NO_BUILD_TARGETS"
Write-Host "  NO_TEST_TARGETS"
Write-Host "  NO_RUNTIME_TARGETS"

exit 0
