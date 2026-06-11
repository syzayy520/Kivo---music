# =============================================================================
# validation_smoke_entry.ps1
# Contract: P0-D lightweight validation smoke entry
# =============================================================================

param(
    [string]$ProjectRoot
)

$ErrorActionPreference = "Continue"

if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
}
$ProjectRoot = (Resolve-Path $ProjectRoot).Path
$ValidationRoot = Join-Path $ProjectRoot "tools\validation"

function Invoke-SmokeScript {
    param(
        [Parameter(Mandatory = $true)][string]$Name,
        [Parameter(Mandatory = $true)][string]$ScriptName,
        [switch]$AllowToolchainBlocked
    )

    Write-Host "-----------------------------------------------------------------------------"
    Write-Host "Running: $Name"
    Write-Host "-----------------------------------------------------------------------------"

    $scriptPath = Join-Path $ValidationRoot $ScriptName
    if (-not (Test-Path $scriptPath)) {
        Write-Host "Missing validation script: $scriptPath"
        return [PSCustomObject]@{ Name = $Name; Result = "FAIL_MISSING_SCRIPT"; ExitCode = 1 }
    }

    $output = & powershell -ExecutionPolicy Bypass -File $scriptPath -ProjectRoot $ProjectRoot 2>&1
    $exitCode = $LASTEXITCODE

    if ($output) {
        $output | ForEach-Object { Write-Host $_ }
    }

    if ($exitCode -eq 0) {
        return [PSCustomObject]@{ Name = $Name; Result = "PASS"; ExitCode = $exitCode }
    }

    if ($AllowToolchainBlocked -and $output -match "BLOCKED_BY_LOCAL_TOOLCHAIN") {
        return [PSCustomObject]@{ Name = $Name; Result = "BLOCKED_BY_LOCAL_TOOLCHAIN"; ExitCode = $exitCode }
    }

    return [PSCustomObject]@{ Name = $Name; Result = "FAIL"; ExitCode = $exitCode }
}

Write-Host "============================================================================="
Write-Host "KivoAudioCoreCpp Validation Smoke Entry"
Write-Host "============================================================================="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host "VALIDATION_ROOT: $ValidationRoot"
Write-Host "Date: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')"
Write-Host "============================================================================="
Write-Host ""

$results = @()
$results += Invoke-SmokeScript -Name "Git State Probe" -ScriptName "git_state_probe.ps1"
if ($results[-1].Result -ne "PASS") {
    Write-Host "CLASSIFICATION: FAIL_SMOKE_GIT_STATE"
    exit 1
}

$results += Invoke-SmokeScript -Name "Environment Probe" -ScriptName "environment_probe.ps1" -AllowToolchainBlocked
$environmentResult = $results[-1].Result

Write-Host "-----------------------------------------------------------------------------"
Write-Host "Running: git diff --check"
Write-Host "-----------------------------------------------------------------------------"
git -C $ProjectRoot diff --check
if ($LASTEXITCODE -ne 0) {
    Write-Host "CLASSIFICATION: FAIL_SMOKE_DIFF_CHECK"
    exit 1
}

Write-Host ""
Write-Host "============================================================================="
Write-Host "VALIDATION SMOKE SUMMARY"
Write-Host "============================================================================="
foreach ($result in $results) {
    Write-Host ("  {0,-22} {1} (exit {2})" -f $result.Name, $result.Result, $result.ExitCode)
}
Write-Host ""

if ($environmentResult -eq "BLOCKED_BY_LOCAL_TOOLCHAIN") {
    Write-Host "CLASSIFICATION: BLOCKED_BY_LOCAL_TOOLCHAIN"
    exit 1
}

if ($environmentResult -ne "PASS") {
    Write-Host "CLASSIFICATION: FAIL_SMOKE_ENVIRONMENT"
    exit 1
}

Write-Host "CLASSIFICATION: PASS_VALIDATION_SMOKE"
exit 0
