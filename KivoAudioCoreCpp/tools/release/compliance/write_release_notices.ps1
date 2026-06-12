param(
    [Parameter(Mandatory = $true)][string]$RuntimeDirectory,
    [Parameter(Mandatory = $true)][string]$FfmpegRoot,
    [Parameter(Mandatory = $true)][string]$Version
)

$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..\..")).Path
. (Join-Path $ProjectRoot "tools\release\foundation\release_foundation.ps1")

$ffmpeg = Join-Path $FfmpegRoot "bin\ffmpeg.exe"
$versionLine = (& $ffmpeg -version | Select-Object -First 1).Trim()
$notice = @"
Kivo Audio Core $Version - Third-Party Notices
================================================

Kivo Audio Core is proprietary software.

FFmpeg
------
This product uses dynamically linked FFmpeg libraries for audio demux,
decoding, and PCM conversion:

- avcodec-61.dll
- avformat-61.dll
- avutil-59.dll
- swresample-5.dll

Build: $versionLine
Provider: BtbN/FFmpeg-Builds
License: GNU Lesser General Public License version 3 or later
Upstream: https://ffmpeg.org/
Build source: https://github.com/BtbN/FFmpeg-Builds

The FFmpeg DLLs are separate, replaceable shared libraries. Kivo Audio Core
does not modify FFmpeg source. The complete LGPL text is included at
licenses/FFmpeg-LGPL-3.0.txt.

Codec patent rights are not granted by this notice. Commercial distribution
requires separate legal review for target markets and enabled codecs.
"@

$sourceOffer = @"
FFmpeg Corresponding Source Information
========================================

Binary revision: n7.1.4-39-ga5faeca88f-20260611
FFmpeg source revision: a5faeca88f
Build scripts: https://github.com/BtbN/FFmpeg-Builds
FFmpeg source: https://github.com/FFmpeg/FFmpeg

For at least the period required by the LGPL, Kivo Music must retain and make
available the exact corresponding source and build scripts for the shipped
FFmpeg binaries. This release-candidate package records the revision and build
configuration but is not a substitute for the commercial source archive.
"@

$noticeDirectory = Join-Path $RuntimeDirectory "notices"
Write-KivoText -Path (Join-Path $noticeDirectory "THIRD_PARTY_NOTICES.txt") `
    -Value ($notice + [Environment]::NewLine)
Write-KivoText -Path (Join-Path $noticeDirectory "FFMPEG_SOURCE_OFFER.txt") `
    -Value ($sourceOffer + [Environment]::NewLine)

$installer = [ordered]@{
    schema = "kivo.installer-payload.v1"
    version = $Version
    required_files = @(
        "bin/kivo_audio_core.dll",
        "bin/avcodec-61.dll",
        "bin/avformat-61.dll",
        "bin/avutil-59.dll",
        "bin/swresample-5.dll",
        "licenses/FFmpeg-LGPL-3.0.txt",
        "notices/THIRD_PARTY_NOTICES.txt",
        "notices/FFMPEG_SOURCE_OFFER.txt"
    )
    rules = @(
        "Install all DLLs in one private application directory.",
        "Do not register the core DLL as a system-wide COM component.",
        "Keep FFmpeg DLLs replaceable by ABI-compatible user copies.",
        "Display or link the third-party notices from the product installer.",
        "Destroy all ABI handles before upgrade, rollback, or DLL replacement."
    )
}
Write-KivoJson -Path (
    Join-Path $RuntimeDirectory "manifest\installer-payload.json") `
    -Value $installer

Write-Host "RELEASE_NOTICES_READY"
