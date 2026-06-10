# =============================================================================
# check_genealogy_gate.ps1
# Genealogy Gate — Directory Structure Validation
# =============================================================================
# Validates that only allowed directories exist in PROJECT_ROOT.
# P0-B allowed: docs/, tools/gates/
# P0-B forbidden: src/, core/, platform/, decode/, playback/, tests/, etc.
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

Write-Host "=== Genealogy Gate ==="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host ""

# Allowed top-level items for P0-C
$allowedTopLevel = @(
    "README.md",
    "CMakeLists.txt",
    "CMakePresets.json",
    ".editorconfig",
    ".gitignore",
    ".clang-format",
    "docs",
    "tools",
    "include",
    "src",
    "tests",
    ".build"
)

# Forbidden directories that indicate runtime/implementation (P0-C scope)
# NOTE: src/, tests/, include/ are now authorized for P0-C contract scope
$forbiddenDirs = @(
    "platform",
    "decode",
    "playback",
    "dsp",
    "diagnostics",
    "examples",
    "demo",
    "sample_player",
    "third_party",
    "vendor",
    "bin",
    "lib"
)

$violations = @()

# Check for forbidden directories
foreach ($dir in $forbiddenDirs) {
    $dirPath = Join-Path $ProjectRoot $dir
    if (Test-Path $dirPath) {
        $violations += "FORBIDDEN_DIRECTORY: $dir/ exists (not authorized for P0-C)"
    }
}

# Check for unauthorized top-level items
$actualItems = Get-ChildItem -Path $ProjectRoot -Force | Where-Object { $_.Name -ne ".git" }
foreach ($item in $actualItems) {
    $found = $false
    foreach ($allowed in $allowedTopLevel) {
        if ($item.Name -eq $allowed) {
            $found = $true
            break
        }
    }
    if (-not $found) {
        $violations += "UNAUTHORIZED_ITEM: $($item.Name) (not in allowed P0-B list)"
    }
}

# Check docs structure
$docsPath = Join-Path $ProjectRoot "docs"
if (Test-Path $docsPath) {
    $docsItems = Get-ChildItem -Path $docsPath -Directory
    foreach ($item in $docsItems) {
        if ($item.Name -notin @("architecture", "closeout", "tasks")) {
            $violations += "UNAUTHORIZED_DOCS_DIR: docs/$($item.Name)/"
        }
    }
}

# Check tools structure
$toolsPath = Join-Path $ProjectRoot "tools"
if (Test-Path $toolsPath) {
    $toolsItems = Get-ChildItem -Path $toolsPath -Directory
    foreach ($item in $toolsItems) {
        if ($item.Name -ne "gates") {
            $violations += "UNAUTHORIZED_TOOLS_DIR: tools/$($item.Name)/"
        }
    }
}

# Report
Write-Host "--- Allowed Top-Level Items ---"
foreach ($a in $allowedTopLevel) {
    Write-Host "  $a"
}
Write-Host ""

if ($violations.Count -eq 0) {
    Write-Host "PASS"
    Write-Host "Directory structure follows P0-C genealogy."
    exit 0
} else {
    Write-Host "FAIL"
    Write-Host "Violations found ($($violations.Count)):"
    foreach ($v in $violations) {
        Write-Host "  - $v"
    }
    exit 1
}
