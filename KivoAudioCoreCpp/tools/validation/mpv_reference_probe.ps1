# =============================================================================
# mpv_reference_probe.ps1
# Contract: P0-R reference-only mpv benchmark probe
# =============================================================================

param(
    [string]$MpvPath,
    [string]$MediaPath
)

$ErrorActionPreference = "Stop"

function Find-MpvExecutable {
    param([string]$ExplicitPath)

    if ($ExplicitPath) {
        if (Test-Path $ExplicitPath) {
            return (Resolve-Path $ExplicitPath).Path
        }
        return $null
    }

    $command = Get-Command mpv -ErrorAction SilentlyContinue
    if ($command) {
        return $command.Source
    }

    return $null
}

$ResolvedMpv = Find-MpvExecutable -ExplicitPath $MpvPath

Write-Host "============================================================================="
Write-Host "KivoAudioCoreCpp mpv Reference Probe"
Write-Host "============================================================================="
Write-Host "Mode: reference-only"
Write-Host "Date: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')"
Write-Host "============================================================================="
Write-Host ""

if (-not $ResolvedMpv) {
    Write-Host "mpv executable was not found."
    Write-Host "CLASSIFICATION: BLOCKED_BY_LOCAL_MPV_INSTALL"
    exit 1
}

Write-Host "mpv: $ResolvedMpv"
& $ResolvedMpv --version
if ($LASTEXITCODE -ne 0) {
    Write-Host "CLASSIFICATION: FAIL_MPV_VERSION_PROBE"
    exit 1
}

if (-not $MediaPath) {
    Write-Host "No media path was supplied. Version probe only."
    Write-Host "CLASSIFICATION: PASS_MPV_REFERENCE_VERSION_ONLY"
    exit 0
}

if (-not (Test-Path $MediaPath)) {
    Write-Host "Media path not found: $MediaPath"
    Write-Host "CLASSIFICATION: BLOCKED_BY_LOCAL_REFERENCE_MEDIA"
    exit 1
}

$ResolvedMedia = (Resolve-Path $MediaPath).Path
$mpvArgs = @(
    "--no-config",
    "--no-video",
    "--ao=null",
    "--ao-null-untimed",
    "--term-status-msg=KIVO_MPV_REFERENCE",
    "--msg-level=all=warn",
    "--",
    $ResolvedMedia
)

Write-Host "Media: $ResolvedMedia"
Write-Host "Options: --no-config --no-video --ao=null --untimed"
Write-Host ""
& $ResolvedMpv @mpvArgs
if ($LASTEXITCODE -ne 0) {
    Write-Host "CLASSIFICATION: FAIL_MPV_REFERENCE_PLAYBACK"
    exit 1
}

Write-Host "CLASSIFICATION: PASS_MPV_REFERENCE_PLAYBACK"
exit 0
