# =============================================================================
# gate_validation_entry.ps1
# Contract: P0-D gate validation entry
# =============================================================================

param(
    [string]$ProjectRoot
)

$ErrorActionPreference = "Continue"

if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
}
$ProjectRoot = (Resolve-Path $ProjectRoot).Path
$GateRoot = Join-Path $ProjectRoot "tools\gates"

$gates = @(
    @{ Name = "Forbidden Token Gate"; Script = "check_forbidden_token_gate.ps1" },
    @{ Name = "Genealogy Gate"; Script = "check_genealogy_gate.ps1" },
    @{ Name = "Contract Genealogy Gate"; Script = "check_contract_genealogy_gate.ps1" },
    @{ Name = "No Empty Forest Gate"; Script = "check_no_empty_forest_gate.ps1" },
    @{ Name = "No Audio Runtime Gate"; Script = "check_no_audio_runtime_gate.ps1" },
    @{ Name = "Policy Substance Gate"; Script = "check_policy_substance_gate.ps1" },
    @{ Name = "Dependency / License Gate"; Script = "check_dependency_license_gate.ps1" },
    @{ Name = "Toolchain / Configure Gate"; Script = "check_toolchain_configure_gate.ps1" },
    @{ Name = "Audio Core Scope Lock Gate"; Script = "check_audio_core_scope_lock_gate.ps1" }
)

Write-Host "============================================================================="
Write-Host "KivoAudioCoreCpp Gate Validation Entry"
Write-Host "============================================================================="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host "GATE_ROOT: $GateRoot"
Write-Host "Date: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')"
Write-Host "============================================================================="
Write-Host ""

$results = @()
$hasFailure = $false

foreach ($gate in $gates) {
    Write-Host "-----------------------------------------------------------------------------"
    Write-Host "Running: $($gate.Name)"
    Write-Host "-----------------------------------------------------------------------------"

    $scriptPath = Join-Path $GateRoot $gate.Script
    if (-not (Test-Path $scriptPath)) {
        Write-Host "Missing gate script: $scriptPath"
        $results += [PSCustomObject]@{ Name = $gate.Name; Result = "FAIL_MISSING_SCRIPT"; ExitCode = 1 }
        $hasFailure = $true
        continue
    }

    $output = & powershell -ExecutionPolicy Bypass -File $scriptPath -ProjectRoot $ProjectRoot 2>&1
    $exitCode = $LASTEXITCODE

    if ($output) {
        $output | ForEach-Object { Write-Host $_ }
    }

    $classification = "FAIL"
    if ($exitCode -eq 0) {
        if ($output -match "ENVIRONMENT_BLOCKED") {
            $classification = "ENVIRONMENT_BLOCKED"
        } elseif ($output -match "PASS_CONFIGURE_SMOKE") {
            $classification = "PASS_CONFIGURE_SMOKE"
        } else {
            $classification = "PASS"
        }
    } else {
        $hasFailure = $true
    }

    $results += [PSCustomObject]@{ Name = $gate.Name; Result = $classification; ExitCode = $exitCode }
    Write-Host ""
}

Write-Host "============================================================================="
Write-Host "GATE VALIDATION SUMMARY"
Write-Host "============================================================================="
foreach ($result in $results) {
    Write-Host ("  {0,-40} {1} (exit {2})" -f $result.Name, $result.Result, $result.ExitCode)
}
Write-Host ""

if ($hasFailure) {
    Write-Host "CLASSIFICATION: FAIL_GATE_VALIDATION"
    exit 1
}

Write-Host "CLASSIFICATION: PASS_GATE_VALIDATION"
exit 0
