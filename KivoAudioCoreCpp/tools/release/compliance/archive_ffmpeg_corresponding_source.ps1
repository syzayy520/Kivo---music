param(
    [Parameter(Mandatory = $true)][string]$FfmpegSourceRoot,
    [Parameter(Mandatory = $true)][string]$BuildScriptsRoot,
    [string]$OutputDirectory = "out\compliance\ffmpeg-source",
    [string]$ExpectedFfmpegRevision = "a5faeca88f",
    [string]$BinaryRevision = "n7.1.4-39-ga5faeca88f-20260611"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..\..")).Path
. (Join-Path $ProjectRoot "tools\release\foundation\release_foundation.ps1")

function Get-CleanGitSource {
    param(
        [Parameter(Mandatory = $true)][string]$Root,
        [Parameter(Mandatory = $true)][string]$Label
    )
    $resolved = (Resolve-Path -LiteralPath $Root).Path
    $commit = (git -C $resolved rev-parse HEAD).Trim()
    if ($LASTEXITCODE -ne 0 -or -not $commit) {
        throw "$Label is not a readable git checkout: $resolved"
    }
    $status = git -C $resolved status --porcelain
    if ($LASTEXITCODE -ne 0 -or $status) {
        throw "$Label checkout must be clean before archival: $resolved"
    }
    return [ordered]@{
        root = $resolved
        commit = $commit
        remote = (git -C $resolved remote get-url origin).Trim()
    }
}

function Export-GitTree {
    param(
        [Parameter(Mandatory = $true)]$Metadata,
        [Parameter(Mandatory = $true)][string]$Destination,
        [Parameter(Mandatory = $true)][string]$TemporaryArchive
    )
    New-Item -ItemType Directory -Path $Destination -Force | Out-Null
    git -C $Metadata.root archive `
        --format=tar `
        "--output=$TemporaryArchive" `
        $Metadata.commit
    if ($LASTEXITCODE -ne 0) {
        throw "git archive failed for $($Metadata.root)"
    }
    tar -xf $TemporaryArchive -C $Destination
    if ($LASTEXITCODE -ne 0) {
        throw "Archive extraction failed for $($Metadata.root)"
    }
    Remove-Item -LiteralPath $TemporaryArchive -Force
}

$ffmpeg = Get-CleanGitSource `
    -Root $FfmpegSourceRoot `
    -Label "FFmpeg source"
$buildScripts = Get-CleanGitSource `
    -Root $BuildScriptsRoot `
    -Label "FFmpeg build scripts"

if (-not $ffmpeg.commit.StartsWith(
        $ExpectedFfmpegRevision,
        [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "FFmpeg revision mismatch. Expected $ExpectedFfmpegRevision, found $($ffmpeg.commit)."
}

$outRoot = Join-Path $ProjectRoot "out"
New-Item -ItemType Directory -Path $outRoot -Force | Out-Null
$output = Assert-KivoPathInside `
    -Path (Join-Path $ProjectRoot $OutputDirectory) `
    -Parent $outRoot
$work = Reset-KivoDirectory `
    -Path (Join-Path $outRoot "compliance-work\ffmpeg-source") `
    -AllowedParent $outRoot
New-Item -ItemType Directory -Path $output -Force | Out-Null

$payload = Join-Path $work "payload"
$sourceDestination = Join-Path $payload "ffmpeg-source"
$scriptsDestination = Join-Path $payload "build-scripts"
Export-GitTree `
    -Metadata $ffmpeg `
    -Destination $sourceDestination `
    -TemporaryArchive (Join-Path $work "ffmpeg-source.tar")
Export-GitTree `
    -Metadata $buildScripts `
    -Destination $scriptsDestination `
    -TemporaryArchive (Join-Path $work "build-scripts.tar")

$licenseCandidates = @(
    "COPYING.LGPLv3",
    "COPYING.LGPLv2.1",
    "LICENSE.md"
)
$presentLicenses = @(
    $licenseCandidates |
        Where-Object {
            Test-Path -LiteralPath (Join-Path $sourceDestination $_)
        }
)
if ($presentLicenses.Count -eq 0) {
    throw "FFmpeg source archive does not contain an expected LGPL/license file."
}

$manifest = [ordered]@{
    schema = "kivo.ffmpeg-corresponding-source.v1"
    binary_revision = $BinaryRevision
    expected_ffmpeg_revision = $ExpectedFfmpegRevision
    ffmpeg = [ordered]@{
        commit = $ffmpeg.commit
        upstream = $ffmpeg.remote
        clean_checkout = $true
        licenses = $presentLicenses
    }
    build_scripts = [ordered]@{
        commit = $buildScripts.commit
        upstream = $buildScripts.remote
        clean_checkout = $true
    }
    modifications = "none recorded by this repository"
    distribution_note =
        "External legal approval and long-term custody remain required."
}
Write-KivoJson `
    -Path (Join-Path $payload "corresponding-source-manifest.json") `
    -Value $manifest

$archiveName = "ffmpeg-$ExpectedFfmpegRevision-corresponding-source.zip"
$archivePath = Join-Path $output $archiveName
New-KivoDeterministicZip `
    -SourceDirectory $payload `
    -ArchivePath $archivePath

$evidence = [ordered]@{
    schema = "kivo.ffmpeg-source-archive-evidence.v1"
    archive = $archiveName
    sha256 = Get-KivoSha256 -Path $archivePath
    size = (Get-Item -LiteralPath $archivePath).Length
    ffmpeg_commit = $ffmpeg.commit
    build_scripts_commit = $buildScripts.commit
    legal_approval = "BLOCKED_EXTERNAL_REVIEW"
    custody = "REQUIRES_EXTERNAL_RETENTION_RECORD"
}
Write-KivoJson `
    -Path (Join-Path $output "ffmpeg-source-archive-evidence.json") `
    -Value $evidence

Write-Host "FFMPEG_CORRESPONDING_SOURCE_ARCHIVED: $archivePath"
Write-Host "SHA256: $($evidence.sha256)"
Write-Host "CLASSIFICATION: SOURCE_ARCHIVE_READY_LEGAL_AND_CUSTODY_OPEN"
