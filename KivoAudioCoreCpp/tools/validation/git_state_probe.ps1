# =============================================================================
# git_state_probe.ps1
# Contract: P0-D git state validation probe
# =============================================================================

param(
    [string]$ProjectRoot,
    [string]$RemoteRef
)

$ErrorActionPreference = "Stop"

if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
}
$ProjectRoot = (Resolve-Path $ProjectRoot).Path

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

function Invoke-ProjectGit {
    param([Parameter(ValueFromRemainingArguments = $true)][string[]]$Arguments)

    & git -c "safe.directory=$GitSafeDirectory" @Arguments
}

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

    $branch = Invoke-ProjectGit branch --show-current
    if (-not $RemoteRef) {
        $RemoteRef = Invoke-ProjectGit rev-parse --abbrev-ref --symbolic-full-name "@{upstream}" 2>$null
        if (-not $RemoteRef) {
            $RemoteRef = "origin/master"
        }
    }

    $status = Invoke-ProjectGit status --short
    $head = Invoke-ProjectGit rev-parse HEAD
    $remote = Invoke-ProjectGit rev-parse $RemoteRef

    Write-Host "Branch: $branch"
    Write-Host "HEAD:   $head"
    Write-Host ("{0}: {1}" -f $RemoteRef, $remote)
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
