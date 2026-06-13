param(
    [Parameter(Mandatory = $true)][string]$BuildDirectory,
    [Parameter(Mandatory = $true)][string]$FfmpegRoot,
    [Parameter(Mandatory = $true)][string]$StageDirectory
)

$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..\..")).Path
. (Join-Path $ProjectRoot "tools\release\foundation\release_foundation.ps1")

$stageParent = Split-Path -Parent $StageDirectory
$StageDirectory = Reset-KivoDirectory `
    -Path $StageDirectory `
    -AllowedParent $stageParent

$runtimeBin = Join-Path $StageDirectory "runtime\bin"
$sdkHostInclude = Join-Path $StageDirectory "runtime\sdk\include\kivo\host"
$licenses = Join-Path $StageDirectory "runtime\licenses"
$symbols = Join-Path $StageDirectory "symbols"
New-Item -ItemType Directory `
    -Path $runtimeBin,$sdkHostInclude,$licenses,$symbols `
    -Force | Out-Null

$requiredBinaries = @(
    (Join-Path $BuildDirectory "kivo_audio_core.dll"),
    (Join-Path $FfmpegRoot "bin\avcodec-61.dll"),
    (Join-Path $FfmpegRoot "bin\avformat-61.dll"),
    (Join-Path $FfmpegRoot "bin\avutil-59.dll"),
    (Join-Path $FfmpegRoot "bin\swresample-5.dll")
)
foreach ($binary in $requiredBinaries) {
    if (-not (Test-Path -LiteralPath $binary)) {
        throw "Required release binary is missing: $binary"
    }
    Copy-Item -LiteralPath $binary -Destination $runtimeBin
}

$headerRoot = Join-Path $ProjectRoot "include\kivo\host\abi"
Copy-Item -LiteralPath $headerRoot -Destination $sdkHostInclude -Recurse

$ffmpegLicense = Join-Path $FfmpegRoot "LICENSE.txt"
Copy-Item -LiteralPath $ffmpegLicense `
    -Destination (Join-Path $licenses "FFmpeg-LGPL-3.0.txt")

$pdb = Join-Path $BuildDirectory "kivo_audio_core.pdb"
if (-not (Test-Path -LiteralPath $pdb)) {
    throw "Release symbol file is missing: $pdb"
}
Copy-Item -LiteralPath $pdb -Destination $symbols

Write-Host "RELEASE_PAYLOAD_STAGED: $StageDirectory"
