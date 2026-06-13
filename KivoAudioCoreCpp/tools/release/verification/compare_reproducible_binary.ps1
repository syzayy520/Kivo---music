param(
    [Parameter(Mandatory = $true)][string]$FirstBuild,
    [Parameter(Mandatory = $true)][string]$SecondBuild,
    [Parameter(Mandatory = $true)][string]$OutputPath
)

$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..\..")).Path
. (Join-Path $ProjectRoot "tools\release\foundation\release_foundation.ps1")

$first = Join-Path $FirstBuild "kivo_audio_core.dll"
$second = Join-Path $SecondBuild "kivo_audio_core.dll"
$firstHash = Get-KivoSha256 -Path $first
$secondHash = Get-KivoSha256 -Path $second
$matched = $firstHash -eq $secondHash

Write-KivoJson -Path $OutputPath -Value ([ordered]@{
    schema = "kivo.reproducibility.v1"
    artifact = "kivo_audio_core.dll"
    first_sha256 = $firstHash
    second_sha256 = $secondHash
    byte_identical = $matched
})

if (-not $matched) {
    throw "Release DLL is not byte-identical across clean builds."
}
Write-Host "REPRODUCIBLE_BINARY_PASS: $firstHash"
