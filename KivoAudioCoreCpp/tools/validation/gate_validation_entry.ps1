# =============================================================================
# gate_validation_entry.ps1
# Contract: validation entry for active audio core gates
# =============================================================================

param(
    [string]$ProjectRoot
)

$ErrorActionPreference = "Continue"

if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
}
$ProjectRoot = (Resolve-Path $ProjectRoot).Path
$GateRunnerPath = Join-Path $ProjectRoot "tools\gates\run_cpp_audio_core_gates.ps1"

Write-Host "============================================================================="
Write-Host "KivoAudioCoreCpp Gate Validation Entry"
Write-Host "============================================================================="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host "GATE_RUNNER: $GateRunnerPath"
Write-Host "Date: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')"
Write-Host "============================================================================="
Write-Host ""

if (-not (Test-Path -LiteralPath $GateRunnerPath)) {
    Write-Host "Missing gate runner: $GateRunnerPath"
    Write-Host "CLASSIFICATION: FAIL_GATE_VALIDATION_MISSING_RUNNER"
    exit 1
}

$output = & powershell -ExecutionPolicy Bypass -File $GateRunnerPath -ProjectRoot $ProjectRoot 2>&1
$exitCode = $LASTEXITCODE

if ($output) {
    $output | ForEach-Object { Write-Host $_ }
}

if ($exitCode -ne 0) {
    Write-Host "CLASSIFICATION: FAIL_GATE_VALIDATION"
    exit 1
}

if ($output -match "OVERALL:\s+ENVIRONMENT_BLOCKED") {
    Write-Host "CLASSIFICATION: BLOCKED_GATE_VALIDATION_ENVIRONMENT"
    exit 0
}

if ($output -match "OVERALL:\s+PASS") {
    Write-Host "CLASSIFICATION: PASS_GATE_VALIDATION"
    exit 0
}

Write-Host "CLASSIFICATION: FAIL_GATE_VALIDATION_UNKNOWN_RESULT"
exit 1
