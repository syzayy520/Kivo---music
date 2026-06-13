param(
    [string]$OutputDirectory = "out\self-test\ffmpeg-source-bundle"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..\..\..\..")).Path
. (Join-Path $ProjectRoot "tools\release\foundation\release_foundation.ps1")

$outRoot = Join-Path $ProjectRoot "out"
New-Item -ItemType Directory -Path $outRoot -Force | Out-Null
$root = Reset-KivoDirectory `
    -Path (Join-Path $ProjectRoot $OutputDirectory) `
    -AllowedParent $outRoot

function New-FixtureRepository {
    param(
        [Parameter(Mandatory = $true)][string]$Path,
        [Parameter(Mandatory = $true)][hashtable]$Files
    )

    New-Item -ItemType Directory -Path $Path -Force | Out-Null
    & git -C $Path init --initial-branch=main | Out-Null
    & git -C $Path config user.email "source-selftest@invalid.local"
    & git -C $Path config user.name "Kivo Source Self-Test"
    foreach ($entry in $Files.GetEnumerator()) {
        $target = Join-Path $Path $entry.Key
        New-Item -ItemType Directory `
            -Path (Split-Path -Parent $target) -Force | Out-Null
        [System.IO.File]::WriteAllText(
            $target,
            [string]$entry.Value,
            [System.Text.UTF8Encoding]::new($false))
    }
    & git -C $Path add --all
    & git -C $Path commit -m "fixture" | Out-Null
    & git -C $Path remote add origin $Path
    return (& git -C $Path rev-parse HEAD).Trim()
}

$ffmpegRoot = Join-Path $root "ffmpeg-fixture"
$scriptsRoot = Join-Path $root "scripts-fixture"
$ffmpegCommit = New-FixtureRepository `
    -Path $ffmpegRoot `
    -Files @{
        "COPYING.LGPLv2.1" = "fixture LGPL text"
        "libavcodec\codec.c" = "int codec_fixture = 1;"
        "README.md" = "FFmpeg fixture"
    }
$scriptsCommit = New-FixtureRepository `
    -Path $scriptsRoot `
    -Files @{
        "build\build.ps1" = "Write-Host 'fixture build'"
        "LICENSE" = "fixture build-script license"
        "README.md" = "Build scripts fixture"
    }

$acquisitionRelative = "$OutputDirectory\acquired"
& (Join-Path $ProjectRoot `
    "tools\release\compliance\source\acquisition\acquire_ffmpeg_source.ps1") `
    -Revision $ffmpegCommit `
    -RepositoryUrl $ffmpegRoot `
    -OutputDirectory $acquisitionRelative `
    -AllowNonOfficialRepository

$scriptsAcquisitionRelative = "$OutputDirectory\acquired-scripts"
& (Join-Path $ProjectRoot `
    "tools\release\compliance\source\acquisition\acquire_ffmpeg_build_scripts.ps1") `
    -Revision $scriptsCommit `
    -RepositoryUrl $scriptsRoot `
    -OutputDirectory $scriptsAcquisitionRelative `
    -AllowNonOfficialRepository

$archiveRelative = "$OutputDirectory\archive"
& (Join-Path $ProjectRoot `
    "tools\release\compliance\source\archival\archive_ffmpeg_corresponding_source.ps1") `
    -FfmpegSourceRoot $ffmpegRoot `
    -BuildScriptsRoot $scriptsRoot `
    -OutputDirectory $archiveRelative `
    -ExpectedFfmpegRevision $ffmpegCommit `
    -BinaryRevision "fixture-binary"

$archiveDirectory = Join-Path $ProjectRoot $archiveRelative
$archive = Get-ChildItem -LiteralPath $archiveDirectory -Filter "*.zip" |
    Select-Object -First 1
$evidence = Join-Path $archiveDirectory `
    "ffmpeg-source-archive-evidence.json"

$secondArchiveRelative = "$OutputDirectory\archive-second"
& (Join-Path $ProjectRoot `
    "tools\release\compliance\source\archival\archive_ffmpeg_corresponding_source.ps1") `
    -FfmpegSourceRoot $ffmpegRoot `
    -BuildScriptsRoot $scriptsRoot `
    -OutputDirectory $secondArchiveRelative `
    -ExpectedFfmpegRevision $ffmpegCommit `
    -BinaryRevision "fixture-binary"
$secondArchive = Get-ChildItem `
    -LiteralPath (Join-Path $ProjectRoot $secondArchiveRelative) `
    -Filter "*.zip" |
    Select-Object -First 1
if ((Get-KivoSha256 -Path $archive.FullName) -ne
    (Get-KivoSha256 -Path $secondArchive.FullName)) {
    throw "Identical source inputs produced different archive bytes."
}

$report = Join-Path $root "verification-report.json"
& (Join-Path $ProjectRoot `
    "tools\release\compliance\source\verification\verify_ffmpeg_source_bundle.ps1") `
    -ArchivePath $archive.FullName `
    -EvidencePath $evidence `
    -ExpectedFfmpegRevision $ffmpegCommit `
    -ExpectedBinaryRevision "fixture-binary" `
    -ReportPath $report

$tamperedRoot = Join-Path $root "tampered"
New-Item -ItemType Directory -Path $tamperedRoot -Force | Out-Null
$tamperedArchive = Join-Path $tamperedRoot $archive.Name
$tamperedEvidence = Join-Path $tamperedRoot `
    "ffmpeg-source-archive-evidence.json"
Copy-Item -LiteralPath $archive.FullName -Destination $tamperedArchive
Copy-Item -LiteralPath $evidence -Destination $tamperedEvidence
[System.IO.File]::AppendAllText($tamperedArchive, "tampered")
$previousErrorAction = $ErrorActionPreference
$ErrorActionPreference = "Continue"
$failureOutput = & powershell -NoProfile -ExecutionPolicy Bypass -File `
    (Join-Path $ProjectRoot `
        "tools\release\compliance\source\verification\verify_ffmpeg_source_bundle.ps1") `
    -ArchivePath $tamperedArchive `
    -EvidencePath $tamperedEvidence `
    -ExpectedFfmpegRevision $ffmpegCommit 2>&1
$failureExitCode = $LASTEXITCODE
$ErrorActionPreference = $previousErrorAction
if ($failureExitCode -eq 0) {
    throw "Tampered source bundle was accepted."
}

$maliciousRoot = Join-Path $root "malicious"
New-Item -ItemType Directory -Path $maliciousRoot -Force | Out-Null
$maliciousArchive = Join-Path $maliciousRoot "malicious.zip"
Add-Type -AssemblyName System.IO.Compression
$stream = [System.IO.File]::Open(
    $maliciousArchive,
    [System.IO.FileMode]::CreateNew)
try {
    $zip = [System.IO.Compression.ZipArchive]::new(
        $stream,
        [System.IO.Compression.ZipArchiveMode]::Create,
        $false)
    try {
        $entry = $zip.CreateEntry("../escape.txt")
        $writer = [System.IO.StreamWriter]::new($entry.Open())
        try {
            $writer.Write("escape")
        } finally {
            $writer.Dispose()
        }
    } finally {
        $zip.Dispose()
    }
} finally {
    $stream.Dispose()
}
$maliciousEvidence = Join-Path $maliciousRoot `
    "ffmpeg-source-archive-evidence.json"
Write-KivoJson -Path $maliciousEvidence -Value ([ordered]@{
    schema = "kivo.ffmpeg-source-archive-evidence.v2"
    archive = "malicious.zip"
    sha256 = Get-KivoSha256 -Path $maliciousArchive
    size = (Get-Item -LiteralPath $maliciousArchive).Length
    ffmpeg_commit = $ffmpegCommit
    build_scripts_commit = $scriptsCommit
    manifest_sha256 = ("0" * 64)
})
$ErrorActionPreference = "Continue"
$maliciousOutput = & powershell -NoProfile -ExecutionPolicy Bypass -File `
    (Join-Path $ProjectRoot `
        "tools\release\compliance\source\verification\verify_ffmpeg_source_bundle.ps1") `
    -ArchivePath $maliciousArchive `
    -EvidencePath $maliciousEvidence `
    -ExpectedFfmpegRevision $ffmpegCommit 2>&1
$maliciousExitCode = $LASTEXITCODE
$ErrorActionPreference = $previousErrorAction
if ($maliciousExitCode -eq 0 -or
    (Test-Path -LiteralPath `
        (Join-Path $ProjectRoot "out\compliance-work\escape.txt"))) {
    throw "Path-traversal source archive was not safely rejected."
}

Write-Host "SOURCE_BUNDLE_TOOLCHAIN_SELFTEST_PASS"
exit 0
