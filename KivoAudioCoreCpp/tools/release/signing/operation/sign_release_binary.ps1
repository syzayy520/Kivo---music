param(
    [Parameter(Mandatory = $true)][string]$RuntimeDirectory,
    [Parameter(Mandatory = $true)][string]$StatusPath,
    [string]$CertificateThumbprint,
    [string]$TimestampUrl = "http://timestamp.digicert.com",
    [switch]$AllowUnsigned
)

$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..\..\..")).Path
. (Join-Path $ProjectRoot "tools\release\foundation\release_foundation.ps1")
. (Join-Path $ProjectRoot "tools\validation\import_msvc_environment.ps1")

$binary = Join-Path $RuntimeDirectory "bin\kivo_audio_core.dll"
if (-not (Test-Path -LiteralPath $binary -PathType Leaf)) {
    throw "Release binary is missing: $binary"
}
if (-not $CertificateThumbprint) {
    Write-KivoJson -Path $StatusPath -Value ([ordered]@{
        schema = "kivo.signing-status.v2"
        status = "blocked_no_certificate"
        artifact = "bin/kivo_audio_core.dll"
        signed = $false
    })
    if ($AllowUnsigned) {
        Write-Host "SIGNING_BLOCKED_NO_CERTIFICATE: unsigned RC allowed"
        exit 0
    }
    throw "A code-signing certificate thumbprint is required."
}

if (-not (Import-KivoMsvcEnvironment)) {
    throw "Windows SDK signing environment is unavailable."
}
$certificate = Get-ChildItem Cert:\CurrentUser\My -CodeSigningCert |
    Where-Object {
        $_.Thumbprint -eq $CertificateThumbprint -and
        $_.HasPrivateKey -and
        $_.NotAfter -gt (Get-Date)
    } |
    Select-Object -First 1
if (-not $certificate) {
    throw "Usable code-signing certificate not found: $CertificateThumbprint"
}

signtool.exe sign /sha1 $CertificateThumbprint /fd SHA256 `
    /tr $TimestampUrl /td SHA256 $binary
if ($LASTEXITCODE -ne 0) {
    throw "Authenticode signing failed."
}
$verificationPath = Join-Path (Split-Path -Parent $StatusPath) `
    "authenticode-verification.json"
& (Join-Path $ProjectRoot `
    "tools\release\signing\verification\verify_authenticode_signature.ps1") `
    -ArtifactPath $binary `
    -ReportPath $verificationPath `
    -ExpectedThumbprint $certificate.Thumbprint

Write-KivoJson -Path $StatusPath -Value ([ordered]@{
    schema = "kivo.signing-status.v2"
    status = "signed_and_verified"
    artifact = "bin/kivo_audio_core.dll"
    signed = $true
    certificate_thumbprint = $certificate.Thumbprint
    certificate_subject = $certificate.Subject
    certificate_expiry = $certificate.NotAfter.ToUniversalTime().ToString("o")
    timestamp_url = $TimestampUrl
    artifact_sha256 = Get-KivoSha256 -Path $binary
    verification_report = "manifest/authenticode-verification.json"
})
Write-Host "SIGNING_PASS"
