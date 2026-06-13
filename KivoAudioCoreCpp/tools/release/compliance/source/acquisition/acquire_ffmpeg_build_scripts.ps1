param(
    [Parameter(Mandatory = $true)][string]$Revision,
    [string]$RepositoryUrl = "https://github.com/BtbN/FFmpeg-Builds.git",
    [string]$OutputDirectory =
        "out\compliance\ffmpeg-build-scripts-acquisition",
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
$official = "https://github.com/BtbN/FFmpeg-Builds.git"
$sanitizedRemote = Get-KivoSanitizedRemote -Remote $RepositoryUrl
if (-not $AllowNonOfficialRepository -and $sanitizedRemote -ne $official) {
    throw "Build-script acquisition accepts only the approved upstream remote."
}

$outRoot = Join-Path $ProjectRoot "out"
New-Item -ItemType Directory -Path $outRoot -Force | Out-Null
$output = Reset-KivoDirectory `
    -Path (Join-Path $ProjectRoot $OutputDirectory) `
    -AllowedParent $outRoot
$checkout = Join-Path $output "source"

& git clone --filter=blob:none --no-checkout -- $RepositoryUrl $checkout
if ($LASTEXITCODE -ne 0) {
    throw "FFmpeg build-script clone failed."
}
& git -C $checkout checkout --detach $Revision
if ($LASTEXITCODE -ne 0) {
    throw "Requested build-script revision is unavailable: $Revision"
}

$metadata = Get-KivoCleanGitCheckout `
    -Root $checkout `
    -Label "Acquired FFmpeg build scripts"
Assert-KivoRevisionMatch -Actual $metadata.commit -Expected $Revision
$licenseCandidates = @("LICENSE", "LICENSE.md", "COPYING")
$licenses = @($licenseCandidates | Where-Object {
    Test-Path -LiteralPath (Join-Path $checkout $_) -PathType Leaf
})
if ($licenses.Count -eq 0) {
    throw "Build-script checkout has no recognized license file."
}
$inventory = @(Get-KivoSourceInventory `
    -PayloadRoot $checkout `
    -RelativeRoots @("."))

Write-KivoJson `
    -Path (Join-Path $output "acquisition-evidence.json") `
    -Value ([ordered]@{
        schema = "kivo.ffmpeg-build-scripts-acquisition.v1"
        requested_revision = $Revision.ToLowerInvariant()
        resolved_commit = $metadata.commit
        upstream = $metadata.remote
        clean_checkout = $true
        detached_head = $true
        licenses = $licenses
        file_count = $inventory.Count
        files = $inventory
    })

Write-Host "FFMPEG_BUILD_SCRIPTS_ACQUIRED: $checkout"
Write-Host "RESOLVED_COMMIT: $($metadata.commit)"
Write-Host "CLASSIFICATION: FFMPEG_BUILD_SCRIPTS_ACQUISITION_PASS"
