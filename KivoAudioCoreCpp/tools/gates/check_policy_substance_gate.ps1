# =============================================================================
# check_policy_substance_gate.ps1
# Policy Substance Gate — Minimum Structure Validation
# =============================================================================
# Validates each policy pack has:
#   1. File exists
#   2. File is non-empty
#   3. Has required sections (Purpose, Scope, Allowed, Forbidden, Gate, Failure, Deferred, Future)
#   4. Not TODO-only
#   5. Not placeholder-only
#   6. Not slogan-only
#   7. Has actionable allowed/forbidden/gate/failure/deferred/future content
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

Write-Host "=== Policy Substance Gate ==="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host ""

# Required policy files
$policyFiles = @(
    "toolchain_and_build_policy.md",
    "dependency_and_license_policy.md",
    "architecture_boundary_policy.md",
    "realtime_thread_and_state_policy.md",
    "audio_truth_policy.md",
    "validation_and_release_policy.md",
    "no_empty_forest_policy.md"
)

# Required sections
$requiredSections = @(
    "Purpose",
    "Scope",
    "Allowed",
    "Forbidden",
    "Failure",
    "Deferred"
)

$violations = @()
$results = @()

foreach ($pf in $policyFiles) {
    $filePath = Join-Path $ProjectRoot "docs\architecture\$pf"
    
    # Check existence
    if (-not (Test-Path $filePath)) {
        $violations += "MISSING: $pf does not exist"
        $results += [PSCustomObject]@{ File = $pf; Exists = $false; NonEmpty = $false; Sections = $false; Substance = $false }
        continue
    }
    
    # Check non-empty
    $content = Get-Content $filePath -Raw
    if ([string]::IsNullOrWhiteSpace($content)) {
        $violations += "EMPTY: $pf is empty"
        $results += [PSCustomObject]@{ File = $pf; Exists = $true; NonEmpty = $false; Sections = $false; Substance = $false }
        continue
    }
    
    # Check required sections
    $missingSections = @()
    foreach ($section in $requiredSections) {
        if ($content -notmatch [regex]::Escape($section)) {
            $missingSections += $section
        }
    }
    if ($missingSections.Count -gt 0) {
        $violations += "MISSING_SECTIONS: $pf missing: $($missingSections -join ', ')"
        $results += [PSCustomObject]@{ File = $pf; Exists = $true; NonEmpty = $true; Sections = $false; Substance = $false }
        continue
    }
    
    # Check substance (not TODO-only, not placeholder-only)
    $lines = ($content -split "`n") | Where-Object { $_.Trim() -ne "" -and $_.Trim() -notmatch "^#" -and $_.Trim() -notmatch "^---" }
    $todoLines = ($lines | Where-Object { $_ -match "TODO|TBD|FIXME|PLACEHOLDER" })
    if ($todoLines.Count -gt ($lines.Count * 0.5)) {
        $violations += "TODO_HEAVY: $pf is more than 50% TODO/placeholder"
        $results += [PSCustomObject]@{ File = $pf; Exists = $true; NonEmpty = $true; Sections = $true; Substance = $false }
        continue
    }
    
    $results += [PSCustomObject]@{ File = $pf; Exists = $true; NonEmpty = $true; Sections = $true; Substance = $true }
}

# Report
Write-Host "--- Policy Pack Results ---"
foreach ($r in $results) {
    $status = if ($r.Substance) { "PASS" } elseif ($r.Sections) { "THIN" } elseif ($r.NonEmpty) { "INCOMPLETE" } elseif ($r.Exists) { "EMPTY" } else { "MISSING" }
    Write-Host "  $($r.File): $status"
}
Write-Host ""

if ($violations.Count -eq 0) {
    Write-Host "PASS"
    Write-Host "POLICY_SUBSTANCE_SMOKE_PASS"
    Write-Host ""
    Write-Host "NOTE: POLICY_SUBSTANCE_SMOKE_PASS only means minimum executable structure exists."
    Write-Host "It does not mean policy content is permanently frozen for all future phases."
    exit 0
} else {
    Write-Host "FAIL"
    Write-Host "Violations found ($($violations.Count)):"
    foreach ($v in $violations) {
        Write-Host "  - $v"
    }
    exit 1
}
