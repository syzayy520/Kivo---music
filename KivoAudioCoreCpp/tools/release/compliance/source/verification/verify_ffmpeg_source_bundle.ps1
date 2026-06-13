param(
    [Parameter(Mandatory = $true)][string]$ArchivePath,
    [Parameter(Mandatory = $true)][string]$EvidencePath,
    [Parameter(Mandatory = $true)][string]$ExpectedFfmpegRevision,
    [string]$ExpectedBinaryRevision,
    [string]$ReportPath
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..\..\..\..")).Path
. (Join-Path $ProjectRoot "tools\release\foundation\release_foundation.ps1")
. (Join-Path $PSScriptRoot "..\foundation\source_bundle_foundation.ps1")

$archive = (Resolve-Path -LiteralPath $ArchivePath).Path
$evidenceFile = (Resolve-Path -LiteralPath $EvidencePath).Path
$evidence = Read-KivoJson -Path $evidenceFile
$errors = [System.Collections.Generic.List[string]]::new()
$resolvedFfmpegCommit = $null
$resolvedBuildScriptsCommit = $null
$resolvedBinaryRevision = $null
$resolvedFileCount = 0

if ($evidence.schema -ne "kivo.ffmpeg-source-archive-evidence.v2") {
    $errors.Add("Unsupported archive evidence schema.")
}
if ($evidence.archive -ne (Split-Path -Leaf $archive)) {
    $errors.Add("Archive name differs from evidence.")
}
if ($evidence.sha256 -ne (Get-KivoSha256 -Path $archive)) {
    $errors.Add("Archive SHA-256 differs from evidence.")
}
if ([int64]$evidence.size -ne (Get-Item -LiteralPath $archive).Length) {
    $errors.Add("Archive size differs from evidence.")
}

$outRoot = Join-Path $ProjectRoot "out"
New-Item -ItemType Directory -Path $outRoot -Force | Out-Null
$work = Reset-KivoDirectory `
    -Path (Join-Path $outRoot "compliance-work\source-verification") `
    -AllowedParent $outRoot
try {
    Expand-KivoSafeSourceArchive `
        -ArchivePath $archive `
        -Destination $work
    $manifestPath = Join-Path $work "corresponding-source-manifest.json"
    if (-not (Test-Path -LiteralPath $manifestPath -PathType Leaf)) {
        $errors.Add("Corresponding-source manifest is missing.")
    } else {
        $manifest = Read-KivoJson -Path $manifestPath
        if ($manifest.schema -ne "kivo.ffmpeg-corresponding-source.v2") {
            $errors.Add("Unsupported corresponding-source schema.")
        }
        $resolvedFfmpegCommit = [string]$manifest.ffmpeg.commit
        $resolvedBuildScriptsCommit =
            [string]$manifest.build_scripts.commit
        $resolvedBinaryRevision = [string]$manifest.binary_revision
        $resolvedFileCount = [int]$manifest.file_count
        try {
            Assert-KivoRevisionMatch `
                -Actual ([string]$manifest.ffmpeg.commit) `
                -Expected $ExpectedFfmpegRevision
        } catch {
            $errors.Add($_.Exception.Message)
        }
        if ($ExpectedBinaryRevision -and
            $manifest.binary_revision -ne $ExpectedBinaryRevision) {
            $errors.Add("Binary revision differs from expected value.")
        }
        if ($evidence.ffmpeg_commit -ne $manifest.ffmpeg.commit -or
            $evidence.build_scripts_commit -ne
                $manifest.build_scripts.commit) {
            $errors.Add("Evidence commits differ from the bundle manifest.")
        }
        if ($evidence.manifest_sha256 -ne
            (Get-KivoSha256 -Path $manifestPath)) {
            $errors.Add("Manifest SHA-256 differs from evidence.")
        }
        try {
            $null = Get-KivoFfmpegLicenses `
                -SourceRoot (Join-Path $work "ffmpeg-source")
        } catch {
            $errors.Add($_.Exception.Message)
        }

        $actualFiles = @(Get-KivoSourceInventory `
            -PayloadRoot $work `
            -RelativeRoots @("ffmpeg-source", "build-scripts"))
        $expectedFiles = @($manifest.files)
        if ($actualFiles.Count -ne [int]$manifest.file_count -or
            $actualFiles.Count -ne $expectedFiles.Count) {
            $errors.Add("Source inventory file count differs.")
        } else {
            for ($index = 0; $index -lt $actualFiles.Count; $index++) {
                $actual = $actualFiles[$index]
                $expected = $expectedFiles[$index]
                if ($actual.path -ne $expected.path -or
                    [int64]$actual.size -ne [int64]$expected.size -or
                    $actual.sha256 -ne $expected.sha256) {
                    $errors.Add("Source inventory mismatch: $($actual.path)")
                    break
                }
            }
        }
    }
} catch {
    $errors.Add($_.Exception.Message)
}

$report = [ordered]@{
    schema = "kivo.ffmpeg-source-verification.v1"
    verified = $errors.Count -eq 0
    archive = Split-Path -Leaf $archive
    archive_sha256 = Get-KivoSha256 -Path $archive
    expected_ffmpeg_revision = $ExpectedFfmpegRevision
    ffmpeg_commit = $resolvedFfmpegCommit
    build_scripts_commit = $resolvedBuildScriptsCommit
    binary_revision = $resolvedBinaryRevision
    file_count = $resolvedFileCount
    errors = @($errors)
}
if ($ReportPath) {
    Write-KivoJson -Path $ReportPath -Value $report
}
if ($errors.Count -ne 0) {
    throw "FFmpeg source bundle verification failed: $($errors -join '; ')"
}

Write-Host "FFMPEG_SOURCE_BUNDLE_VERIFICATION_PASS"
