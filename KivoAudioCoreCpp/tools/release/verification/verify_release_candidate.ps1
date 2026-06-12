param(
    [Parameter(Mandatory = $true)][string]$RuntimeDirectory,
    [Parameter(Mandatory = $true)][string]$ReportPath
)

$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..\..")).Path
. (Join-Path $ProjectRoot "tools\release\foundation\release_foundation.ps1")
. (Join-Path $ProjectRoot "tools\validation\import_msvc_environment.ps1")

if (-not (Import-KivoMsvcEnvironment)) {
    throw "MSVC verification environment is unavailable."
}

$manifestPath = Join-Path $RuntimeDirectory "manifest\build-manifest.json"
$manifest = Read-KivoJson -Path $manifestPath
$errors = [System.Collections.Generic.List[string]]::new()

foreach ($file in $manifest.files) {
    $path = Join-Path $RuntimeDirectory ($file.path -replace "/", "\")
    if (-not (Test-Path -LiteralPath $path)) {
        $errors.Add("Missing payload file: $($file.path)")
        continue
    }
    if ((Get-KivoSha256 -Path $path) -ne $file.sha256) {
        $errors.Add("Hash mismatch: $($file.path)")
    }
}

$dll = Join-Path $RuntimeDirectory "bin\kivo_audio_core.dll"
$expectedExports = @(
    "kivo_audio_create",
    "kivo_audio_destroy",
    "kivo_audio_get_capabilities",
    "kivo_audio_get_snapshot",
    "kivo_audio_open_source",
    "kivo_audio_pump",
    "kivo_audio_submit_command"
)
$exportOutput = dumpbin.exe /nologo /exports $dll
$exportText = $exportOutput -join [Environment]::NewLine
$actualExports = @(
    [regex]::Matches(
        $exportText,
        "(?m)^\s+\d+\s+[0-9A-F]+\s+[0-9A-F]+\s+" +
            "(kivo_audio_[a-z_]+)\s*$") |
        ForEach-Object { $_.Groups[1].Value } |
        Sort-Object
)
if (Compare-Object $expectedExports $actualExports) {
    $errors.Add(
        "Export surface differs from the locked ABI: " +
        ($actualExports -join ", "))
}

$headers = dumpbin.exe /nologo /headers $dll
$headerText = $headers -join [Environment]::NewLine
foreach ($requiredHeader in @(
    "Dynamic base",
    "NX compatible",
    "Control Flow Guard",
    "CET Compatible")) {
    if ($headerText -notmatch [regex]::Escape($requiredHeader)) {
        $errors.Add("PE hardening flag missing: $requiredHeader")
    }
}

$version = (Get-Item -LiteralPath $dll).VersionInfo
if (-not $version.FileVersion.StartsWith("1.0.0")) {
    $errors.Add("Unexpected DLL file version: $($version.FileVersion)")
}

$pdbInRuntime = Get-ChildItem -LiteralPath $RuntimeDirectory -Recurse -File `
    -Filter "*.pdb"
if ($pdbInRuntime) {
    $errors.Add("Runtime package contains private PDB symbols.")
}

$manifestText = [System.IO.File]::ReadAllText(
    $manifestPath,
    [System.Text.UTF8Encoding]::new($false))
$containsProjectRoot =
    $manifestText -match [regex]::Escape($ProjectRoot)
$containsWindowsUserPath =
    $manifestText -match "[A-Za-z]:\\\\Users\\\\"
if ($containsProjectRoot -or $containsWindowsUserPath) {
    $errors.Add("Build manifest exposes an absolute local path.")
}

$signed = [bool]$manifest.signing.signed
if ($signed) {
    $signatureReport = Join-Path (Split-Path -Parent $ReportPath) `
        "authenticode-verification.json"
    & (Join-Path $ProjectRoot `
        "tools\release\signing\verification\verify_authenticode_signature.ps1") `
        -ArtifactPath $dll `
        -ReportPath $signatureReport `
        -ExpectedThumbprint $manifest.signing.certificate_thumbprint
}

$commercialBlockers = [System.Collections.Generic.List[string]]::new()
if (-not $signed) {
    $commercialBlockers.Add("Kivo DLL is not Authenticode signed.")
}
$sourceVerified = $false
$source = $manifest.dependency.corresponding_source
if ($source -and
    $source.schema -eq "kivo.ffmpeg-source-verification.v1" -and
    [bool]$source.verified -and
    $source.binary_revision -eq
        $manifest.dependency.ffmpeg_binary_revision) {
    $sourceVerified = $true
}
if (-not $sourceVerified) {
    $commercialBlockers.Add(
        "Exact FFmpeg corresponding-source archive is not bound to this release.")
}
$commercialBlockers.Add(
    "FFmpeg distribution legal approval is an external signed record.")
$commercialBlockers.Add(
    "USB, Bluetooth, active HDMI, physical transition, and ten-hour lab rows remain open.")
$commercialBlockers.Add(
    "The product host installer must consume and sign this payload.")

$report = [ordered]@{
    schema = "kivo.release-verification.v1"
    engineering_verified = $errors.Count -eq 0
    commercial_release_approved =
        $errors.Count -eq 0 -and $commercialBlockers.Count -eq 0
    errors = @($errors)
    commercial_blockers = @($commercialBlockers)
    export_count = $actualExports.Count
    file_version = $version.FileVersion
    product_version = $version.ProductVersion
    signed = $signed
    corresponding_source_verified = $sourceVerified
}
Write-KivoJson -Path $ReportPath -Value $report

if ($errors.Count -ne 0) {
    throw "Release candidate verification failed: $($errors -join '; ')"
}
Write-Host "RELEASE_ENGINEERING_VERIFICATION_PASS"
if ($commercialBlockers.Count -ne 0) {
    Write-Host "COMMERCIAL_RELEASE_BLOCKED: $($commercialBlockers.Count) external gate(s)"
}
