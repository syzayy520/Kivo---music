# =============================================================================
# environment_probe.ps1
# Contract: P0-D validation environment probe
# =============================================================================

param(
    [string]$ProjectRoot
)

$ErrorActionPreference = "Continue"

if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
}
$ProjectRoot = (Resolve-Path $ProjectRoot).Path

. (Join-Path $PSScriptRoot "import_msvc_environment.ps1")
$msvcEnvironmentImported = Import-KivoMsvcEnvironment

function Get-ToolStatus {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Name
    )

    $command = Get-Command $Name -ErrorAction SilentlyContinue
    if ($command) {
        return [PSCustomObject]@{
            Name = $Name
            Found = $true
            Source = $command.Source
        }
    }

    return [PSCustomObject]@{
        Name = $Name
        Found = $false
        Source = ""
    }
}

Write-Host "============================================================================="
Write-Host "KivoAudioCoreCpp Validation Environment Probe"
Write-Host "============================================================================="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host "Date: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')"
Write-Host "============================================================================="
Write-Host ""
Write-Host ("  {0,-10} {1}" -f "msvc-env", $(if ($msvcEnvironmentImported) { "READY" } else { "UNAVAILABLE" }))
Write-Host ""

$tools = @(
    "git",
    "cmake",
    "cl",
    "clang++",
    "g++",
    "ninja"
)

$statuses = foreach ($tool in $tools) {
    Get-ToolStatus -Name $tool
}

foreach ($status in $statuses) {
    if ($status.Found) {
        Write-Host ("  {0,-10} FOUND   {1}" -f $status.Name, $status.Source)
    } else {
        Write-Host ("  {0,-10} MISSING" -f $status.Name)
    }
}

Write-Host ""

$hasGit = @($statuses | Where-Object { $_.Name -eq "git" -and $_.Found }).Count -gt 0
$hasCmake = @($statuses | Where-Object { $_.Name -eq "cmake" -and $_.Found }).Count -gt 0
$hasCompiler = @($statuses | Where-Object {
    ($_.Name -eq "cl" -or $_.Name -eq "clang++" -or $_.Name -eq "g++") -and $_.Found
}).Count -gt 0
$hasNinja = @($statuses | Where-Object { $_.Name -eq "ninja" -and $_.Found }).Count -gt 0

if ($hasGit -and $hasCmake -and $hasCompiler) {
    if ($hasNinja) {
        Write-Host "CLASSIFICATION: PASS_TOOLCHAIN_PROBE_READY_NINJA"
    } else {
        Write-Host "CLASSIFICATION: PASS_TOOLCHAIN_PROBE_READY_COMPILER_NO_NINJA"
    }
    exit 0
}

Write-Host "CLASSIFICATION: BLOCKED_BY_LOCAL_TOOLCHAIN"
exit 1
