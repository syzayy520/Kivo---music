# =============================================================================
# run_cpp_audio_core_gates.ps1
# Aggregate Gate Runner — Runs All P0-C Gates
# =============================================================================
# Runs all gate scripts and reports summary.
# Exit code 0 = all gates passed (or environment blocked).
# Exit code 1 = one or more gates failed.
# =============================================================================

param(
    [string]$ProjectRoot
)

$ErrorActionPreference = "Continue"

# Resolve PROJECT_ROOT
if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
    $ProjectRoot = Resolve-Path $ProjectRoot
}

Write-Host "============================================================================="
Write-Host "KivoAudioCoreCpp — Audio Core Gate Runner (9 Gates)"
Write-Host "============================================================================="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host "Date: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')"
Write-Host "============================================================================="
Write-Host ""

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

$results = @()
$allPassed = $true

foreach ($gate in $gates) {
    Write-Host "-----------------------------------------------------------------------------"
    Write-Host "Running: $($gate.Name)"
    Write-Host "-----------------------------------------------------------------------------"
    
    $scriptPath = Join-Path $PSScriptRoot $gate.Script
    $output = & powershell -ExecutionPolicy Bypass -File $scriptPath -ProjectRoot $ProjectRoot 2>&1
    $exitCode = $LASTEXITCODE
    
    $lastLine = ($output | Where-Object { $_ -match "\S" } | Select-Object -Last 1).ToString().Trim()
    
    $gateResult = "UNKNOWN"
    if ($lastLine -match "PASS" -or $output -match "PASS") {
        $gateResult = "PASS"
    } elseif ($lastLine -match "ENVIRONMENT_BLOCKED" -or $output -match "ENVIRONMENT_BLOCKED") {
        $gateResult = "ENVIRONMENT_BLOCKED"
    } elseif ($lastLine -match "PASS_CONFIGURE_SMOKE" -or $output -match "PASS_CONFIGURE_SMOKE") {
        $gateResult = "PASS_CONFIGURE_SMOKE"
    } else {
        $gateResult = "FAIL"
        $allPassed = $false
    }
    
    $results += [PSCustomObject]@{
        Name = $gate.Name
        Result = $gateResult
        ExitCode = $exitCode
    }
    
    Write-Host ""
}

# Summary
Write-Host "============================================================================="
Write-Host "GATE SUMMARY"
Write-Host "============================================================================="
Write-Host ""

foreach ($r in $results) {
    $statusIcon = switch ($r.Result) {
        "PASS" { "PASS" }
        "PASS_CONFIGURE_SMOKE" { "PASS (configure smoke)" }
        "ENVIRONMENT_BLOCKED" { "BLOCKED (env)" }
        "FAIL" { "FAIL" }
        default { "???" }
    }
    Write-Host ("  {0,-40} {1}" -f $r.Name, $statusIcon)
}

Write-Host ""

if ($allPassed) {
    Write-Host "OVERALL: PASS"
    Write-Host ""
    Write-Host "All P0-C gates passed."
    Write-Host "NOTE: Configure smoke PASS does NOT mean build/test/runtime PASS."
    exit 0
} else {
    $failCount = ($results | Where-Object { $_.Result -eq "FAIL" }).Count
    Write-Host "OVERALL: FAIL"
    Write-Host "$failCount gate(s) failed."
    exit 1
}
