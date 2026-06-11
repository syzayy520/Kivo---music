# =============================================================================
# git_state_probe.ps1
# Contract: P0-D git state validation probe
# =============================================================================

param(
    [string]$ProjectRoot,
    [string]$RemoteRef = "origin/master"
)

$ErrorActionPreference = "Stop"

if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
}
$ProjectRoot = (Resolve-Path $ProjectRoot).Path

Push-Location $ProjectRoot
try {
    Write-Host "============================================================================="
    Write-Host "KivoAudioCoreCpp Git State Probe"
    Write-Host "============================================================================="
    Write-Host "PROJECT_ROOT: $ProjectRoot"
    Write-Host "RemoteRef: $RemoteRef"
    Write-Host "Date: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')"
    Write-Host "============================================================================="
    Write-Host ""

    $status = git status --short
    $branch = git branch --show-current
    $head = git rev-parse HEAD
    $remote = git rev-parse $RemoteRef

    Write-Host "Branch: $branch"
    Write-Host "HEAD:   $head"
    Write-Host "$RemoteRef: $remote"
    Write-Host ""

    if ($status) {
        Write-Host "Working tree is not clean:"
        $status | ForEach-Object { Write-Host $_ }
        Write-Host "CLASSIFICATION: FAIL_GIT_WORKTREE_NOT_CLEAN"
        exit 1
    }

    if ($head -ne $remote) {
        Write-Host "HEAD does not match $RemoteRef."
        Write-Host "CLASSIFICATION: FAIL_GIT_HEAD_REMOTE_MISMATCH"
        exit 1
    }

    Write-Host "CLASSIFICATION: PASS_GIT_STATE_CLEAN_HEAD_EQUALS_REMOTE"
    exit 0
} finally {
    Pop-Location
}
