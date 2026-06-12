param(
    [Parameter(Mandatory = $true)][string]$RuntimeDirectory,
    [Parameter(Mandatory = $true)][string]$FfmpegRoot,
    [Parameter(Mandatory = $true)][string]$Version,
    [Parameter(Mandatory = $true)][string]$GitCommit
)

$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..\..")).Path
. (Join-Path $ProjectRoot "tools\release\foundation\release_foundation.ps1")

$binaryDirectory = Join-Path $RuntimeDirectory "bin"
$ffmpegOutput = & (Join-Path $FfmpegRoot "bin\ffmpeg.exe") -version
$configuration = ($ffmpegOutput | Where-Object {
    $_ -like "configuration:*"
} | Select-Object -First 1).Trim()
$sha = [System.Security.Cryptography.SHA256]::Create()
try {
    $serialBytes = $sha.ComputeHash(
        [System.Text.Encoding]::UTF8.GetBytes($GitCommit))[0..15]
} finally {
    $sha.Dispose()
}
$serialBytes[6] = ($serialBytes[6] -band 0x0f) -bor 0x50
$serialBytes[8] = ($serialBytes[8] -band 0x3f) -bor 0x80
$serialHex = -join ($serialBytes | ForEach-Object { $_.ToString("x2") })
$serialGuid = "{0}-{1}-{2}-{3}-{4}" -f `
    $serialHex.Substring(0, 8), `
    $serialHex.Substring(8, 4), `
    $serialHex.Substring(12, 4), `
    $serialHex.Substring(16, 4), `
    $serialHex.Substring(20, 12)

function New-Component {
    param(
        [string]$Name,
        [string]$ComponentVersion,
        [string]$FileName,
        [string]$LicenseId,
        [string]$BomRef
    )
    $path = Join-Path $binaryDirectory $FileName
    return [ordered]@{
        type = "library"
        "bom-ref" = $BomRef
        name = $Name
        version = $ComponentVersion
        hashes = @(
            [ordered]@{
                alg = "SHA-256"
                content = Get-KivoSha256 -Path $path
            }
        )
        licenses = @(
            [ordered]@{
                license = [ordered]@{ id = $LicenseId }
            }
        )
        properties = @(
            [ordered]@{
                name = "kivo:distribution:file"
                value = "bin/$FileName"
            }
        )
    }
}

$kivo = New-Component `
    -Name "Kivo Audio Core" `
    -ComponentVersion $Version `
    -FileName "kivo_audio_core.dll" `
    -LicenseId "NOASSERTION" `
    -BomRef "pkg:generic/kivo-audio-core@$Version"
$kivo.type = "application"
$kivo.licenses = @(
    [ordered]@{
        license = [ordered]@{ name = "Kivo Proprietary" }
    }
)
$kivo.properties += [ordered]@{
    name = "kivo:source:commit"
    value = $GitCommit
}

$components = @(
    (New-Component "FFmpeg libavcodec" "61.19.101" `
        "avcodec-61.dll" "LGPL-3.0-or-later" `
        "pkg:generic/ffmpeg-libavcodec@61.19.101"),
    (New-Component "FFmpeg libavformat" "61.7.103" `
        "avformat-61.dll" "LGPL-3.0-or-later" `
        "pkg:generic/ffmpeg-libavformat@61.7.103"),
    (New-Component "FFmpeg libavutil" "59.39.100" `
        "avutil-59.dll" "LGPL-3.0-or-later" `
        "pkg:generic/ffmpeg-libavutil@59.39.100"),
    (New-Component "FFmpeg libswresample" "5.3.100" `
        "swresample-5.dll" "LGPL-3.0-or-later" `
        "pkg:generic/ffmpeg-libswresample@5.3.100")
)

$sbom = [ordered]@{
    bomFormat = "CycloneDX"
    specVersion = "1.6"
    serialNumber = "urn:uuid:$serialGuid"
    version = 1
    metadata = [ordered]@{
        component = $kivo
        properties = @(
            [ordered]@{
                name = "kivo:ffmpeg:configuration"
                value = $configuration
            },
            [ordered]@{
                name = "kivo:sbom:scope"
                value = "Shipped core and FFmpeg DLLs; Windows system libraries excluded."
            }
        )
    }
    components = $components
    dependencies = @(
        [ordered]@{
            ref = "pkg:generic/kivo-audio-core@$Version"
            dependsOn = @(
                "pkg:generic/ffmpeg-libavcodec@61.19.101",
                "pkg:generic/ffmpeg-libavformat@61.7.103",
                "pkg:generic/ffmpeg-libavutil@59.39.100",
                "pkg:generic/ffmpeg-libswresample@5.3.100"
            )
        }
    )
}

Write-KivoJson -Path (
    Join-Path $RuntimeDirectory "sbom\kivo-audio-core.cdx.json") `
    -Value $sbom
Write-Host "CYCLONEDX_SBOM_READY"
