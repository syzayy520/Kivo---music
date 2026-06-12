# =============================================================================
# validation_batch_entry.ps1
# Contract: P0-D validation batch entry
# =============================================================================

param(
    [string]$ProjectRoot,
    [switch]$SkipBuild,
    [switch]$SkipGates
)

$ErrorActionPreference = "Continue"

if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
}
$ProjectRoot = (Resolve-Path $ProjectRoot).Path
$ValidationRoot = Join-Path $ProjectRoot "tools\validation"

function Find-GitSafeDirectory {
    param([Parameter(Mandatory = $true)][string]$StartPath)

    $current = (Resolve-Path $StartPath).Path
    while ($current) {
        if (Test-Path (Join-Path $current ".git")) {
            return $current.Replace("\", "/")
        }

        $parent = Split-Path $current -Parent
        if (-not $parent -or $parent -eq $current) {
            break
        }
        $current = $parent
    }

    return $StartPath.Replace("\", "/")
}

$GitSafeDirectory = Find-GitSafeDirectory -StartPath $ProjectRoot

function Invoke-ValidationScript {
    param(
        [Parameter(Mandatory = $true)][string]$Name,
        [Parameter(Mandatory = $true)][string]$ScriptName,
        [switch]$AllowBlocked
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

    if ($AllowBlocked -and $output -match "BLOCKED_BY_LOCAL_TOOLCHAIN") {
        return [PSCustomObject]@{ Name = $Name; Result = "BLOCKED_BY_LOCAL_TOOLCHAIN"; ExitCode = $exitCode }
    }

    return [PSCustomObject]@{ Name = $Name; Result = "FAIL"; ExitCode = $exitCode }
}

Write-Host "============================================================================="
Write-Host "KivoAudioCoreCpp Validation Batch Entry"
Write-Host "============================================================================="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host "VALIDATION_ROOT: $ValidationRoot"
Write-Host "SkipBuild: $SkipBuild"
Write-Host "SkipGates: $SkipGates"
Write-Host "Date: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')"
Write-Host "============================================================================="
Write-Host ""

$results = @()
$results += Invoke-ValidationScript -Name "Git State Probe" -ScriptName "git_state_probe.ps1"
if ($results[-1].Result -ne "PASS") {
    Write-Host "CLASSIFICATION: FAIL_VALIDATION_BATCH_GIT_PRECONDITION"
    exit 1
}

$environmentResult = Invoke-ValidationScript -Name "Environment Probe" -ScriptName "environment_probe.ps1" -AllowBlocked
$results += $environmentResult
if ($environmentResult.Result -eq "BLOCKED_BY_LOCAL_TOOLCHAIN") {
    Write-Host "CLASSIFICATION: BLOCKED_BY_LOCAL_TOOLCHAIN"
    exit 1
}
if ($environmentResult.Result -ne "PASS") {
    Write-Host "CLASSIFICATION: FAIL_VALIDATION_BATCH_ENVIRONMENT_PRECONDITION"
    exit 1
}

if (-not $SkipBuild) {
    $results += Invoke-ValidationScript -Name "Build Validation" -ScriptName "build_validation_entry.ps1"
    if ($results[-1].Result -ne "PASS") {
        Write-Host "CLASSIFICATION: FAIL_VALIDATION_BATCH_BUILD"
        exit 1
    }
}

if (-not $SkipGates) {
    $results += Invoke-ValidationScript -Name "Gate Validation" -ScriptName "gate_validation_entry.ps1"
    if ($results[-1].Result -ne "PASS") {
        Write-Host "CLASSIFICATION: FAIL_VALIDATION_BATCH_GATES"
        exit 1
    }
}

Write-Host "-----------------------------------------------------------------------------"
Write-Host "Running: git diff --check"
Write-Host "-----------------------------------------------------------------------------"
git -c "safe.directory=$GitSafeDirectory" -C $ProjectRoot diff --check
if ($LASTEXITCODE -ne 0) {
    Write-Host "CLASSIFICATION: FAIL_VALIDATION_BATCH_DIFF_CHECK"
    exit 1
}

Write-Host ""
Write-Host "============================================================================="
Write-Host "VALIDATION BATCH SUMMARY"
Write-Host "============================================================================="
foreach ($result in $results) {
    Write-Host ("  {0,-28} {1} (exit {2})" -f $result.Name, $result.Result, $result.ExitCode)
}
Write-Host ""
Write-Host "CLASSIFICATION: PASS_VALIDATION_BATCH"
exit 0
