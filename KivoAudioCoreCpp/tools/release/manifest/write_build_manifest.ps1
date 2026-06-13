param(
    [Parameter(Mandatory = $true)][string]$RuntimeDirectory,
    [Parameter(Mandatory = $true)][string]$BuildDirectory,
    [Parameter(Mandatory = $true)][string]$FfmpegRoot,
    [Parameter(Mandatory = $true)][string]$Version,
    [Parameter(Mandatory = $true)]$GitMetadata,
    [Parameter(Mandatory = $true)][string]$SigningStatusPath,
    [Parameter(Mandatory = $true)][string]$ReproducibilityPath,
    [string]$CorrespondingSourceReportPath
)

$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..\..")).Path
. (Join-Path $ProjectRoot "tools\release\foundation\release_foundation.ps1")

$manifestPath = Join-Path $RuntimeDirectory "manifest\build-manifest.json"
$files = Get-ChildItem -LiteralPath $RuntimeDirectory -Recurse -File |
    Where-Object { $_.FullName -ne $manifestPath } |
    Sort-Object FullName |
    ForEach-Object {
        [ordered]@{
            path = Get-KivoRelativePath `
                -Root $RuntimeDirectory `
                -Path $_.FullName
            size = $_.Length
            sha256 = Get-KivoSha256 -Path $_.FullName
        }
    }

$compilerFile = Get-ChildItem `
    -Path (Join-Path $BuildDirectory "CMakeFiles") `
    -Recurse `
    -Filter "CMakeCXXCompiler.cmake" |
    Select-Object -First 1
$compilerLine = Select-String `
    -Path $compilerFile.FullName `
    -Pattern 'set\(CMAKE_CXX_COMPILER_VERSION "([^"]+)"\)' |
    Select-Object -First 1
$compilerVersion = $compilerLine.Matches[0].Groups[1].Value
$ffmpegVersion = (& (Join-Path $FfmpegRoot "bin\ffmpeg.exe") -version |
    Select-Object -First 1).Trim()
$ffmpegBinaryRevision = if ($ffmpegVersion -match
    "^ffmpeg version ([^\s]+)") {
    $Matches[1]
} else {
    "unknown"
}
$sourceVerification = if ($CorrespondingSourceReportPath -and
    (Test-Path -LiteralPath $CorrespondingSourceReportPath)) {
    Read-KivoJson -Path $CorrespondingSourceReportPath
} else {
    [ordered]@{
        schema = "kivo.ffmpeg-source-verification.v1"
        verified = $false
        status = "not_supplied"
    }
}

$manifest = [ordered]@{
    schema = "kivo.build-manifest.v1"
    product = "Kivo Audio Core"
    version = $Version
    release_channel = "release-candidate"
    abi = "1.0.0"
    source = $GitMetadata
    toolchain = [ordered]@{
        cmake = (cmake --version | Select-Object -First 1).Trim()
        ninja = (ninja --version).Trim()
        msvc = $compilerVersion
        configuration = "Release"
        reproducible_flags = @("/Brepro", "/ZH:SHA_256")
        hardening_flags = @(
            "/guard:cf",
            "/DYNAMICBASE",
            "/NXCOMPAT",
            "/CETCOMPAT"
        )
    }
    dependency = [ordered]@{
        ffmpeg = $ffmpegVersion
        ffmpeg_binary_revision = $ffmpegBinaryRevision
        baseline_archive_sha256 =
            "c8bc13fb4079fc477ecca83985d913d43d1a69efdb974ebc1f37490e603b5b79"
        dynamic_linking = $true
        replaceable_dlls = $true
        corresponding_source = $sourceVerification
    }
    signing = Read-KivoJson -Path $SigningStatusPath
    reproducibility =
        Read-KivoJson -Path $ReproducibilityPath
    privacy = [ordered]@{
        absolute_source_paths_in_manifest = $false
        credentials_in_diagnostics = $false
        diagnostic_payload = "numeric ABI fields only"
    }
    files = @($files)
}

Write-KivoJson -Path $manifestPath -Value $manifest
Write-Host "BUILD_MANIFEST_READY: $manifestPath"
