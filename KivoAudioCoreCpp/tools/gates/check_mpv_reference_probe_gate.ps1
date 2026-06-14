# =============================================================================
# check_mpv_reference_probe_gate.ps1
# P0-R mpv Reference Probe Gate - Static reference-only probe lock
# =============================================================================

param(
    [string]$ProjectRoot
)

$ErrorActionPreference = "Stop"

if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
}
$ProjectRoot = (Resolve-Path $ProjectRoot).Path

Write-Host "=== mpv Reference Probe Gate ==="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host ""

$violations = @()
$probeRelativePath = "tools\validation\mpv_reference_probe.ps1"
$probePath = Join-Path $ProjectRoot $probeRelativePath

if (-not (Test-Path -LiteralPath $probePath)) {
    $violations += "MISSING_MPV_REFERENCE_PROBE: $probeRelativePath"
} else {
    $content = Get-Content -LiteralPath $probePath -Raw
    if ([string]::IsNullOrWhiteSpace($content)) {
        $violations += "EMPTY_MPV_REFERENCE_PROBE: $probeRelativePath"
    }

    $requiredOptions = @(
        "--no-config",
        "--no-video",
        "--ao=null",
        "--ao-null-untimed",
        "--msg-level=all=warn"
    )
    foreach ($option in $requiredOptions) {
        if ($content -notmatch [regex]::Escape($option)) {
            $violations += "MISSING_MPV_REFERENCE_OPTION: $option"
        }
    }

    $requiredClassifications = @(
        "BLOCKED_BY_LOCAL_MPV_INSTALL",
        "PASS_MPV_REFERENCE_VERSION_ONLY",
        "BLOCKED_BY_LOCAL_REFERENCE_MEDIA",
        "FAIL_MPV_REFERENCE_PLAYBACK",
        "PASS_MPV_REFERENCE_PLAYBACK"
    )
    foreach ($classification in $requiredClassifications) {
        if ($content -notmatch [regex]::Escape($classification)) {
            $violations += "MISSING_MPV_REFERENCE_CLASSIFICATION: $classification"
        }
    }
}

$referenceFiles = @(
    "docs\tasks\p0_r_mpv_libmpv_reference_research_family_design_task.md",
    "docs\validation\p0_r_mpv_libmpv_reference_research_2026_06_12.md"
)
foreach ($referenceFile in $referenceFiles) {
    $referencePath = Join-Path $ProjectRoot $referenceFile
    if (-not (Test-Path -LiteralPath $referencePath)) {
        $violations += "MISSING_MPV_REFERENCE_DOC: $referenceFile"
        continue
    }

    $referenceContent = Get-Content -LiteralPath $referencePath -Raw
    if ($referenceContent -notmatch [regex]::Escape("mpv_reference_probe.ps1")) {
        $violations += "MISSING_MPV_REFERENCE_PROBE_DOC_LINK: $referenceFile"
    }
}

if ($violations.Count -eq 0) {
    Write-Host "PASS"
    Write-Host "MPV_REFERENCE_PROBE_GATE_PASSED"
    Write-Host "mpv probe is locked to reference-only options and classifications."
    exit 0
}

Write-Host "FAIL"
Write-Host "MPV_REFERENCE_PROBE_GATE_FAILED"
Write-Host "Violations found ($($violations.Count)):"
foreach ($violation in $violations) {
    Write-Host "  - $violation"
}
exit 1
