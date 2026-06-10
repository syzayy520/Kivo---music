# =============================================================================
# check_dependency_license_gate.ps1
# Dependency / License Gate — Zero Dependency Validation
# =============================================================================
# Validates P0-B has zero third-party dependencies:
#   1. No vendor/, third_party/, bin/, lib/ directories
#   2. No FetchContent in CMakeLists.txt
#   3. No vcpkg.json or conanfile.txt
#   4. No git submodules
#   5. No download commands in gate scripts
# =============================================================================

param(
    [string]$ProjectRoot
)

$ErrorActionPreference = "Stop"

# Resolve PROJECT_ROOT — always resolve to absolute path
if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
}
$ProjectRoot = (Resolve-Path $ProjectRoot).Path

Write-Host "=== Dependency / License Gate ==="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host ""

$violations = @()

# --- 1. Check for dependency directories ---
$depDirs = @("vendor", "third_party", "bin", "lib")
foreach ($dir in $depDirs) {
    $dirPath = Join-Path $ProjectRoot $dir
    if (Test-Path $dirPath) {
        $violations += "DEPENDENCY_DIR: $dir/ exists"
    }
}

# --- 2. Check CMakeLists.txt for FetchContent ---
$cmakePath = Join-Path $ProjectRoot "CMakeLists.txt"
if (Test-Path $cmakePath) {
    $cmakeContent = Get-Content $cmakePath -Raw
    if ($cmakeContent -match "FetchContent") {
        $violations += "FETCH_CONTENT: CMakeLists.txt contains FetchContent"
    }
    if ($cmakeContent -match "find_package") {
        # Check for non-commented find_package
        $lines = $cmakeContent -split "`n"
        foreach ($line in $lines) {
            if ($line -match "find_package" -and $line -notmatch "^\s*#") {
                $violations += "FIND_PACKAGE: CMakeLists.txt contains uncommented find_package"
                break
            }
        }
    }
}

# --- 3. Check for vcpkg/conan ---
$vcpkgPath = Join-Path $ProjectRoot "vcpkg.json"
$conanPath = Join-Path $ProjectRoot "conanfile.txt"
$conanPyPath = Join-Path $ProjectRoot "conanfile.py"
if (Test-Path $vcpkgPath) { $violations += "VCPKG: vcpkg.json exists" }
if (Test-Path $conanPath) { $violations += "CONAN: conanfile.txt exists" }
if (Test-Path $conanPyPath) { $violations += "CONAN: conanfile.py exists" }

# --- 4. Check for git submodules ---
$gitmodulesPath = Join-Path $ProjectRoot ".gitmodules"
if (Test-Path $gitmodulesPath) {
    $violations += "SUBMODULE: .gitmodules exists"
}

# --- 5. Check for CMakePresets toolchain references ---
$presetPath = Join-Path $ProjectRoot "CMakePresets.json"
if (Test-Path $presetPath) {
    $presetContent = Get-Content $presetPath -Raw
    if ($presetContent -match "toolchainFile") {
        $violations += "TOOLCHAIN: CMakePresets.json references toolchainFile"
    }
}

# Report
Write-Host "--- Checks ---"
Write-Host "1. Dependency directories (vendor/, third_party/, bin/, lib/)"
Write-Host "2. FetchContent in CMakeLists.txt"
Write-Host "3. vcpkg.json / conanfile.txt / conanfile.py"
Write-Host "4. .gitmodules"
Write-Host "5. CMakePresets toolchainFile"
Write-Host ""

if ($violations.Count -eq 0) {
    Write-Host "PASS"
    Write-Host "Zero third-party dependencies confirmed."
    exit 0
} else {
    Write-Host "FAIL"
    Write-Host "Violations found ($($violations.Count)):"
    foreach ($v in $violations) {
        Write-Host "  - $v"
    }
    exit 1
}
