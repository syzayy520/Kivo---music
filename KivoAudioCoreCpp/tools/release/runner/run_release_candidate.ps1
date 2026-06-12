param(
    [string]$Version = "1.0.0-rc.1",
    [string]$FfmpegRoot,
    [string]$FfmpegSourceArchive,
    [string]$FfmpegSourceEvidence,
    [string]$ExpectedFfmpegSourceRevision = "a5faeca88f",
    [string]$ExpectedFfmpegBinaryRevision =
        "n7.1.4-39-ga5faeca88f-20260611",
    [string]$CertificateThumbprint,
    [switch]$AllowUnsigned
)

$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..\..")).Path
. (Join-Path $ProjectRoot "tools\release\foundation\release_foundation.ps1")

$git = Assert-KivoCleanGit -ProjectRoot $ProjectRoot
$FfmpegRoot = Get-KivoFfmpegRoot `
    -RequestedRoot $FfmpegRoot `
    -ProjectRoot $ProjectRoot

$outRoot = Join-Path $ProjectRoot "out"
New-Item -ItemType Directory -Path $outRoot -Force | Out-Null
$workRoot = Reset-KivoDirectory `
    -Path (Join-Path $outRoot "release-work\$Version") `
    -AllowedParent $outRoot
$releaseRoot = Reset-KivoDirectory `
    -Path (Join-Path $outRoot "release\$Version") `
    -AllowedParent $outRoot

$buildA = Join-Path $workRoot "build-a"
$buildB = Join-Path $workRoot "build-b"
$stage = Join-Path $workRoot "stage"
$runtime = Join-Path $stage "runtime"
$symbols = Join-Path $stage "symbols"
$reproducibility = Join-Path $runtime "manifest\reproducibility.json"
$signingStatus = Join-Path $runtime "manifest\signing-status.json"
$verification = Join-Path $releaseRoot "verification-report.json"
$sourceVerification = Join-Path $runtime `
    "manifest\ffmpeg-source-verification.json"

if ([bool]$FfmpegSourceArchive -ne [bool]$FfmpegSourceEvidence) {
    throw "FFmpeg source archive and evidence must be supplied together."
}

& (Join-Path $ProjectRoot `
    "tools\release\build\build_release_candidate.ps1") `
    -BuildDirectory $buildA `
    -FfmpegRoot $FfmpegRoot
& (Join-Path $ProjectRoot `
    "tools\release\build\build_release_candidate.ps1") `
    -BuildDirectory $buildB `
    -FfmpegRoot $FfmpegRoot `
    -SkipTests

& (Join-Path $ProjectRoot `
    "tools\release\artifact\stage_release_payload.ps1") `
    -BuildDirectory $buildA `
    -FfmpegRoot $FfmpegRoot `
    -StageDirectory $stage

& (Join-Path $ProjectRoot `
    "tools\release\verification\compare_reproducible_binary.ps1") `
    -FirstBuild $buildA `
    -SecondBuild $buildB `
    -OutputPath $reproducibility

& (Join-Path $ProjectRoot `
    "tools\release\compliance\write_release_notices.ps1") `
    -RuntimeDirectory $runtime `
    -FfmpegRoot $FfmpegRoot `
    -Version $Version
& (Join-Path $ProjectRoot `
    "tools\release\compliance\write_cyclonedx_sbom.ps1") `
    -RuntimeDirectory $runtime `
    -FfmpegRoot $FfmpegRoot `
    -Version $Version `
    -GitCommit $git.commit

if ($FfmpegSourceArchive) {
    & (Join-Path $ProjectRoot `
        "tools\release\compliance\source\verification\verify_ffmpeg_source_bundle.ps1") `
        -ArchivePath $FfmpegSourceArchive `
        -EvidencePath $FfmpegSourceEvidence `
        -ExpectedFfmpegRevision $ExpectedFfmpegSourceRevision `
        -ExpectedBinaryRevision $ExpectedFfmpegBinaryRevision `
        -ReportPath $sourceVerification
}

& (Join-Path $ProjectRoot `
    "tools\release\signing\operation\sign_release_binary.ps1") `
    -RuntimeDirectory $runtime `
    -StatusPath $signingStatus `
    -CertificateThumbprint $CertificateThumbprint `
    -AllowUnsigned:$AllowUnsigned

& (Join-Path $ProjectRoot `
    "tools\release\manifest\write_build_manifest.ps1") `
    -RuntimeDirectory $runtime `
    -BuildDirectory $buildA `
    -FfmpegRoot $FfmpegRoot `
    -Version $Version `
    -GitMetadata $git `
    -SigningStatusPath $signingStatus `
    -ReproducibilityPath $reproducibility `
    -CorrespondingSourceReportPath $sourceVerification

& (Join-Path $ProjectRoot `
    "tools\release\verification\verify_release_candidate.ps1") `
    -RuntimeDirectory $runtime `
    -ReportPath $verification

$runtimeZip = Join-Path $releaseRoot "kivo-audio-core-$Version-win-x64.zip"
$runtimeZipCheck = Join-Path $workRoot "runtime-repro-check.zip"
$symbolsZip = Join-Path $releaseRoot "kivo-audio-core-$Version-symbols.zip"
New-KivoDeterministicZip `
    -SourceDirectory $runtime `
    -ArchivePath $runtimeZip
New-KivoDeterministicZip `
    -SourceDirectory $runtime `
    -ArchivePath $runtimeZipCheck
$runtimeArchiveHash = Get-KivoSha256 $runtimeZip
$runtimeArchiveCheckHash = Get-KivoSha256 $runtimeZipCheck
if ($runtimeArchiveHash -ne $runtimeArchiveCheckHash) {
    throw "Deterministic runtime archive verification failed."
}
New-KivoDeterministicZip `
    -SourceDirectory $symbols `
    -ArchivePath $symbolsZip

$sourceIndex = $null
if ($FfmpegSourceArchive) {
    $sourceDirectory = Join-Path $releaseRoot "corresponding-source"
    New-Item -ItemType Directory -Path $sourceDirectory -Force | Out-Null
    $sourceArchiveCopy = Join-Path $sourceDirectory `
        (Split-Path -Leaf $FfmpegSourceArchive)
    $sourceEvidenceCopy = Join-Path $sourceDirectory `
        (Split-Path -Leaf $FfmpegSourceEvidence)
    Copy-Item -LiteralPath $FfmpegSourceArchive `
        -Destination $sourceArchiveCopy
    Copy-Item -LiteralPath $FfmpegSourceEvidence `
        -Destination $sourceEvidenceCopy
    $sourceResult = Read-KivoJson -Path $sourceVerification
    $sourceIndex = [ordered]@{
        archive = "corresponding-source/$(
            Split-Path -Leaf $sourceArchiveCopy)"
        archive_sha256 = Get-KivoSha256 -Path $sourceArchiveCopy
        evidence = "corresponding-source/$(
            Split-Path -Leaf $sourceEvidenceCopy)"
        evidence_sha256 = Get-KivoSha256 -Path $sourceEvidenceCopy
        ffmpeg_commit = $sourceResult.ffmpeg_commit
        build_scripts_commit = $sourceResult.build_scripts_commit
        verified = [bool]$sourceResult.verified
        custody = "REQUIRES_EXTERNAL_RETENTION_RECORD"
        legal_approval = "BLOCKED_EXTERNAL_REVIEW"
    }
}

$verificationResult = Read-KivoJson -Path $verification
$index = [ordered]@{
    schema = "kivo.release-index.v1"
    version = $Version
    source_commit = $git.commit
    runtime_archive = [ordered]@{
        file = Split-Path -Leaf $runtimeZip
        sha256 = Get-KivoSha256 $runtimeZip
    }
    symbols_archive = [ordered]@{
        file = Split-Path -Leaf $symbolsZip
        sha256 = Get-KivoSha256 $symbolsZip
    }
    engineering_verified = $verificationResult.engineering_verified
    commercial_release_approved =
        $verificationResult.commercial_release_approved
    commercial_blockers = $verificationResult.commercial_blockers
    corresponding_source = $sourceIndex
}
Write-KivoJson -Path (Join-Path $releaseRoot "release-index.json") `
    -Value $index

Write-Host "RELEASE_CANDIDATE_READY: $releaseRoot"
Write-Host "RUNTIME_SHA256: $($index.runtime_archive.sha256)"
Write-Host "SYMBOLS_SHA256: $($index.symbols_archive.sha256)"
if (-not $index.commercial_release_approved) {
    Write-Host "CLASSIFICATION: ENGINEERING_RC_READY_COMMERCIAL_RELEASE_BLOCKED"
} else {
    Write-Host "CLASSIFICATION: COMMERCIAL_RELEASE_APPROVED"
}
