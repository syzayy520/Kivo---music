# =============================================================================
# run_cpp_audio_core_gates.ps1
# Aggregate Gate Runner - Runs All Active Audio Core Gates
# =============================================================================
# Runs all gate scripts and reports summary.
# Exit code 0 = no source gate failed; see OVERALL for pass/blocker class.
# Exit code 1 = one or more gates failed.
# =============================================================================

param(
    [string]$ProjectRoot
)

$ErrorActionPreference = "Continue"

# Keep gate output and child-process diagnostics stable on localized Windows.
chcp 65001 | Out-Null

# Resolve PROJECT_ROOT to an absolute path.
if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
}
$ProjectRoot = (Resolve-Path $ProjectRoot).Path

Write-Host "============================================================================="
Write-Host "KivoAudioCoreCpp - Audio Core Gate Runner (11 Gates)"
Write-Host "============================================================================="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host "Date: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')"
Write-Host "============================================================================="
Write-Host ""

$gates = @(
    @{ Name = "Execution Spec Gate"; Script = "check_execution_spec_gate.ps1" },
    @{ Name = "Forbidden Token Gate"; Script = "check_forbidden_token_gate.ps1" },
    @{ Name = "Genealogy Gate"; Script = "check_genealogy_gate.ps1" },
    @{ Name = "Contract Genealogy Gate"; Script = "check_contract_genealogy_gate.ps1" },
    @{ Name = "No Empty Forest Gate"; Script = "check_no_empty_forest_gate.ps1" },
    @{ Name = "No Audio Runtime Gate"; Script = "check_no_audio_runtime_gate.ps1" },
    @{ Name = "Policy Substance Gate"; Script = "check_policy_substance_gate.ps1" },
    @{ Name = "Dependency / License Gate"; Script = "check_dependency_license_gate.ps1" },
    @{ Name = "mpv Reference Probe Gate"; Script = "check_mpv_reference_probe_gate.ps1" },
    @{ Name = "Toolchain / Configure Gate"; Script = "check_toolchain_configure_gate.ps1" },
    @{ Name = "Audio Core Scope Lock Gate"; Script = "check_audio_core_scope_lock_gate.ps1" }
)

$results = @()
$allPassed = $true
$hasEnvironmentBlocked = $false

foreach ($gate in $gates) {
    Write-Host "-----------------------------------------------------------------------------"
    Write-Host "Running: $($gate.Name)"
    Write-Host "-----------------------------------------------------------------------------"
    
    $scriptPath = Join-Path $PSScriptRoot $gate.Script
    $output = & powershell -ExecutionPolicy Bypass -File $scriptPath -ProjectRoot $ProjectRoot 2>&1
    $exitCode = $LASTEXITCODE
    
    $lastLine = ($output | Where-Object { $_ -match "\S" } | Select-Object -Last 1).ToString().Trim()
    
    $gateResult = "UNKNOWN"
    if ($exitCode -ne 0) {
        $gateResult = "FAIL"
        $allPassed = $false
    } elseif ($lastLine -match "ENVIRONMENT_BLOCKED" -or $output -match "ENVIRONMENT_BLOCKED") {
        $gateResult = "ENVIRONMENT_BLOCKED"
        $hasEnvironmentBlocked = $true
    } elseif ($lastLine -match "PASS_CONFIGURE_SMOKE" -or $output -match "PASS_CONFIGURE_SMOKE") {
        $gateResult = "PASS_CONFIGURE_SMOKE"
    } elseif ($lastLine -match "PASS" -or $output -match "PASS") {
        $gateResult = "PASS"
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

if (-not $allPassed) {
    $failCount = ($results | Where-Object { $_.Result -eq "FAIL" }).Count
    Write-Host "OVERALL: FAIL"
    Write-Host "$failCount gate(s) failed."
    exit 1
} elseif ($hasEnvironmentBlocked) {
    Write-Host "OVERALL: ENVIRONMENT_BLOCKED"
    Write-Host ""
    Write-Host "No source gate failure was detected, but at least one local prerequisite is blocked."
    Write-Host "Do not report this as PASS until the environment blocker is cleared."
    exit 0
} else {
    Write-Host "OVERALL: PASS"
    Write-Host ""
    Write-Host "All active audio core gates passed."
    Write-Host "NOTE: Configure smoke PASS does NOT mean build/test/runtime PASS."
    exit 0
}
