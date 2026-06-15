# =============================================================================
# check_execution_spec_gate.ps1
# Execution Spec Gate - Required workflow anchor validation
# =============================================================================

param(
    [string]$ProjectRoot
)

$ErrorActionPreference = "Stop"

if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
}
$ProjectRoot = (Resolve-Path $ProjectRoot).Path

Write-Host "=== Execution Spec Gate ==="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host ""

$violations = @()
$specRelativePath = "docs\AUDIO_CORE_EXECUTION_SPEC.md"
$specPath = Join-Path $ProjectRoot $specRelativePath

if (-not (Test-Path -LiteralPath $specPath)) {
    $violations += "MISSING_EXECUTION_SPEC: $specRelativePath"
} else {
    $content = Get-Content -LiteralPath $specPath -Raw
    if ([string]::IsNullOrWhiteSpace($content)) {
        $violations += "EMPTY_EXECUTION_SPEC: $specRelativePath"
    }

    $requiredAnchors = @(
        "Source-of-truth Precedence",
        "Ticket Required Header",
        "Ticket Acceptance Form",
        "Machine-checkable Guard Commands"
    )
    foreach ($anchor in $requiredAnchors) {
        if ($content -notmatch [regex]::Escape($anchor)) {
            $violations += "MISSING_EXECUTION_SPEC_ANCHOR: $anchor"
        }
    }

    $requiredClassifications = @(
        "PASS_LOCAL_ENGINEERING_GUARDS",
        "BLOCKED_BY_LOCAL_ENVIRONMENT",
        "BLOCKED_BY_EXTERNAL_EVIDENCE",
        "FAIL_SOURCE_OR_ARCHITECTURE"
    )
    foreach ($classification in $requiredClassifications) {
        if ($content -notmatch [regex]::Escape($classification)) {
            $violations += "MISSING_ACCEPTANCE_CLASSIFICATION: $classification"
        }
    }
}

$referenceFiles = @(
    "docs\architecture\policy_index.md",
    "docs\architecture\audio_core_execution_roadmap.md"
)
foreach ($referenceFile in $referenceFiles) {
    $referencePath = Join-Path $ProjectRoot $referenceFile
    if (-not (Test-Path -LiteralPath $referencePath)) {
        $violations += "MISSING_EXECUTION_SPEC_REFERENCE_OWNER: $referenceFile"
        continue
    }
    $referenceContent = Get-Content -LiteralPath $referencePath -Raw
    if ($referenceContent -notmatch [regex]::Escape("docs/AUDIO_CORE_EXECUTION_SPEC.md")) {
        $violations += "MISSING_EXECUTION_SPEC_REFERENCE: $referenceFile"
    }
}

if ($violations.Count -eq 0) {
    Write-Host "PASS"
    Write-Host "Execution spec anchors are present and referenced."
    exit 0
}

Write-Host "FAIL"
Write-Host "Violations found ($($violations.Count)):"
foreach ($violation in $violations) {
    Write-Host "  - $violation"
}
exit 1
