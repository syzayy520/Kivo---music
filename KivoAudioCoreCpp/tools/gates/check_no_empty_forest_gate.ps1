# =============================================================================
# check_no_empty_forest_gate.ps1
# No Empty Forest Gate — Directory and Policy Content Validation
# =============================================================================

param(
    [string]$ProjectRoot
)

$ErrorActionPreference = "Stop"

if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
}
$ProjectRoot = (Resolve-Path $ProjectRoot).Path

Write-Host "=== No Empty Forest Gate ==="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host ""

$violations = @()
$excludedPathPatterns = @("*\.git\*", "*\.build\*", "*\build\*")

function Test-IsExcludedPath {
    param([string]$Path)
    foreach ($pattern in $excludedPathPatterns) {
        if ($Path -like $pattern) {
            return $true
        }
    }
    return $false
}

function Test-EmptyDirectory {
    param([string]$Path)
    $items = Get-ChildItem -Path $Path -File -Recurse -Force -ErrorAction SilentlyContinue |
        Where-Object { -not (Test-IsExcludedPath $_.FullName) }
    return ($null -eq $items -or $items.Count -eq 0)
}

$dirs = Get-ChildItem -Path $ProjectRoot -Directory -Recurse -Force |
    Where-Object { -not (Test-IsExcludedPath $_.FullName) }

foreach ($dir in $dirs) {
    if (Test-EmptyDirectory -Path $dir.FullName) {
        $relativePath = $dir.FullName.Replace($ProjectRoot, "").TrimStart("\", "/")
        $violations += "EMPTY_DIRECTORY: $relativePath/"
    }
}

$bucketDirs = @("helper", "utils", "common", "misc", "glue", "facade", "types", "defs")
foreach ($dir in $dirs) {
    if ($dir.Name -in $bucketDirs) {
        $relativePath = $dir.FullName.Replace($ProjectRoot, "").TrimStart("\", "/")
        $violations += "BUCKET_DIRECTORY: $relativePath/"
    }
}

$requiredSections = @(
    "Purpose",
    "Scope",
    "Current Phase",
    "Allowed",
    "Forbidden",
    "Gate",
    "Failure",
    "Future Phase",
    "Deferred",
    "Last Updated"
)

$policyFiles = Get-ChildItem -Path (Join-Path $ProjectRoot "docs\architecture") -Filter "*.md" -ErrorAction SilentlyContinue
foreach ($pf in $policyFiles) {
    if ($pf.Name -eq "design_planning_001.md" -or $pf.Name -eq "policy_index.md" -or $pf.Name -eq "phase_closeout_template.md") {
        continue
    }
    $content = Get-Content $pf.FullName -Raw
    $missingSections = @()
    foreach ($section in $requiredSections) {
        if ($content -notmatch [regex]::Escape($section)) {
            $missingSections += $section
        }
    }
    if ($missingSections.Count -gt 0) {
        $violations += "POLICY_MISSING_SECTIONS: $($pf.Name) missing: $($missingSections -join ', ')"
    }
    $lines = ($content -split "`n") | Where-Object { $_.Trim() -ne "" -and $_.Trim() -notmatch "^#" -and $_.Trim() -notmatch "^---" }
    if ($lines.Count -lt 10) {
        $violations += "POLICY_TOO_THIN: $($pf.Name) has less than 10 non-header lines"
    }
}

Write-Host "--- Checks ---"
Write-Host "1. Empty directories: scanning..."
Write-Host "2. Bucket directories..."
Write-Host "3. Policy file required sections..."
Write-Host "Excluding generated directories: .build/, build/"
Write-Host ""

if ($violations.Count -eq 0) {
    Write-Host "PASS"
    Write-Host "No empty forests detected. All policies have required sections."
    exit 0
}

Write-Host "FAIL"
Write-Host "Violations found ($($violations.Count)):"
foreach ($v in $violations) {
    Write-Host "  - $v"
}
exit 1
