# =============================================================================
# check_no_empty_forest_gate.ps1
# No Empty Forest Gate — Directory and Policy Content Validation
# =============================================================================
# Validates:
#   1. No empty directories exist
#   2. Policy files have required sections with non-trivial content
#   3. No helper/utils/common bucket directories
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

Write-Host "=== No Empty Forest Gate ==="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host ""

$violations = @()

# --- Check for empty directories ---
function Test-EmptyDirectory {
    param([string]$Path)
    $items = Get-ChildItem -Path $Path -File -Recurse -Force -ErrorAction SilentlyContinue |
        Where-Object { $_.FullName -notlike "*\.git\*" -and $_.FullName -notlike "*\.build\*" }
    return ($null -eq $items -or $items.Count -eq 0)
}

$dirs = Get-ChildItem -Path $ProjectRoot -Directory -Recurse -Force |
    Where-Object { $_.FullName -notlike "*\.git\*" -and $_.FullName -notlike "*\.build\*" }

foreach ($dir in $dirs) {
    if (Test-EmptyDirectory -Path $dir.FullName) {
        $relativePath = $dir.FullName.Replace($ProjectRoot, "").TrimStart("\", "/")
        $violations += "EMPTY_DIRECTORY: $relativePath/"
    }
}

# --- Check for bucket directories ---
$bucketDirs = @("helper", "utils", "common", "misc", "glue", "facade", "types", "defs")
foreach ($dir in $dirs) {
    if ($dir.Name -in $bucketDirs) {
        $relativePath = $dir.FullName.Replace($ProjectRoot, "").TrimStart("\", "/")
        $violations += "BUCKET_DIRECTORY: $relativePath/"
    }
}

# --- Check policy files have required sections ---
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
        continue  # Skip non-policy files
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
    # Check for TODO-only or placeholder-only
    $lines = ($content -split "`n") | Where-Object { $_.Trim() -ne "" -and $_.Trim() -notmatch "^#" -and $_.Trim() -notmatch "^---" }
    if ($lines.Count -lt 10) {
        $violations += "POLICY_TOO_THIN: $($pf.Name) has less than 10 non-header lines"
    }
}

# Report
Write-Host "--- Checks ---"
Write-Host "1. Empty directories: scanning..."
Write-Host "2. Bucket directories (helper/utils/common/misc/glue/facade/types/defs)..."
Write-Host "3. Policy file required sections..."
Write-Host ""

if ($violations.Count -eq 0) {
    Write-Host "PASS"
    Write-Host "No empty forests detected. All policies have required sections."
    exit 0
} else {
    Write-Host "FAIL"
    Write-Host "Violations found ($($violations.Count)):"
    foreach ($v in $violations) {
        Write-Host "  - $v"
    }
    exit 1
}
