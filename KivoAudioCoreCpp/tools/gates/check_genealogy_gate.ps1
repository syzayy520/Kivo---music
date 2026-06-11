# =============================================================================
# check_genealogy_gate.ps1
# Genealogy Gate — Directory Structure Validation
# =============================================================================

param(
    [string]$ProjectRoot
)

$ErrorActionPreference = "Stop"

if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
}
$ProjectRoot = (Resolve-Path $ProjectRoot).Path

Write-Host "=== Genealogy Gate ==="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host ""

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
    ".build",
    "build"
)

$blockedTopLevelDirs = @(
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

foreach ($dir in $blockedTopLevelDirs) {
    $dirPath = Join-Path $ProjectRoot $dir
    if (Test-Path $dirPath) {
        $violations += "BLOCKED_TOP_LEVEL_DIRECTORY: $dir/ exists"
    }
}

$actualItems = Get-ChildItem -Path $ProjectRoot -Force | Where-Object { $_.Name -ne ".git" }
foreach ($item in $actualItems) {
    if ($item.Name -notin $allowedTopLevel) {
        $violations += "UNAUTHORIZED_ITEM: $($item.Name)"
    }
}

$docsPath = Join-Path $ProjectRoot "docs"
if (Test-Path $docsPath) {
    $docsItems = Get-ChildItem -Path $docsPath -Directory
    foreach ($item in $docsItems) {
        if ($item.Name -notin @("architecture", "closeout", "tasks", "validation")) {
            $violations += "UNAUTHORIZED_DOCS_DIR: docs/$($item.Name)/"
        }
    }
}

$toolsPath = Join-Path $ProjectRoot "tools"
if (Test-Path $toolsPath) {
    $toolsItems = Get-ChildItem -Path $toolsPath -Directory
    foreach ($item in $toolsItems) {
        if ($item.Name -notin @("gates", "validation")) {
            $violations += "UNAUTHORIZED_TOOLS_DIR: tools/$($item.Name)/"
        }
    }
}

Write-Host "--- Allowed Top-Level Items ---"
foreach ($a in $allowedTopLevel) {
    Write-Host "  $a"
}
Write-Host ""

if ($violations.Count -eq 0) {
    Write-Host "PASS"
    Write-Host "Directory structure follows current contract genealogy."
    exit 0
}

Write-Host "FAIL"
Write-Host "Violations found ($($violations.Count)):"
foreach ($v in $violations) {
    Write-Host "  - $v"
}
exit 1
