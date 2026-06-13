param(
    [Parameter(Mandatory = $true)][string]$Revision,
    [string]$RepositoryUrl = "https://git.ffmpeg.org/ffmpeg.git",
    [string]$OutputDirectory = "out\compliance\ffmpeg-acquisition",
    [switch]$AllowNonOfficialRepository
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..\..\..\..")).Path
. (Join-Path $ProjectRoot "tools\release\foundation\release_foundation.ps1")
. (Join-Path $PSScriptRoot "..\foundation\source_bundle_foundation.ps1")

if ($Revision -notmatch "^[0-9a-fA-F]{10,40}$") {
    throw "Revision must contain 10 to 40 hexadecimal characters."
}

$official = @(
    "https://git.ffmpeg.org/ffmpeg.git",
    "https://github.com/FFmpeg/FFmpeg.git"
)
$sanitizedRemote = Get-KivoSanitizedRemote -Remote $RepositoryUrl
if (-not $AllowNonOfficialRepository -and $sanitizedRemote -notin $official) {
    throw "FFmpeg acquisition accepts only an official upstream remote."
}

$outRoot = Join-Path $ProjectRoot "out"
New-Item -ItemType Directory -Path $outRoot -Force | Out-Null
$output = Reset-KivoDirectory `
    -Path (Join-Path $ProjectRoot $OutputDirectory) `
    -AllowedParent $outRoot
$checkout = Join-Path $output "source"

& git clone --filter=blob:none --no-checkout -- $RepositoryUrl $checkout
if ($LASTEXITCODE -ne 0) {
    throw "FFmpeg source clone failed."
}
& git -C $checkout checkout --detach $Revision
if ($LASTEXITCODE -ne 0) {
    throw "Requested FFmpeg revision is unavailable: $Revision"
}

$metadata = Get-KivoCleanGitCheckout `
    -Root $checkout `
    -Label "Acquired FFmpeg source"
Assert-KivoRevisionMatch -Actual $metadata.commit -Expected $Revision
$licenses = @(Get-KivoFfmpegLicenses -SourceRoot $checkout)
$inventory = @(Get-KivoSourceInventory `
    -PayloadRoot $checkout `
    -RelativeRoots @("."))

$evidence = [ordered]@{
    schema = "kivo.ffmpeg-source-acquisition.v1"
    requested_revision = $Revision.ToLowerInvariant()
    resolved_commit = $metadata.commit
    upstream = $metadata.remote
    clean_checkout = $true
    detached_head = $true
    licenses = $licenses
    file_count = $inventory.Count
    files = $inventory
}
Write-KivoJson `
    -Path (Join-Path $output "acquisition-evidence.json") `
    -Value $evidence

Write-Host "FFMPEG_SOURCE_ACQUIRED: $checkout"
Write-Host "RESOLVED_COMMIT: $($metadata.commit)"
Write-Host "CLASSIFICATION: FFMPEG_SOURCE_ACQUISITION_PASS"
