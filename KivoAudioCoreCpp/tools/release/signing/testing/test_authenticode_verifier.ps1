param(
    [string]$OutputDirectory = "out\self-test\authenticode"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..\..\..")).Path
. (Join-Path $ProjectRoot "tools\release\foundation\release_foundation.ps1")

$outRoot = Join-Path $ProjectRoot "out"
New-Item -ItemType Directory -Path $outRoot -Force | Out-Null
$root = Reset-KivoDirectory `
    -Path (Join-Path $ProjectRoot $OutputDirectory) `
    -AllowedParent $outRoot
$fixture = Join-Path $root "signed-fixture.ps1"
[System.IO.File]::WriteAllText(
    $fixture,
    "Write-Output 'Kivo Authenticode fixture'`r`n",
    [System.Text.UTF8Encoding]::new($false))

$certificate = $null
try {
    $certificate = New-SelfSignedCertificate `
        -Type CodeSigningCert `
        -Subject "CN=Kivo Authenticode Self-Test" `
        -CertStoreLocation "Cert:\CurrentUser\My" `
        -KeyAlgorithm RSA `
        -KeyLength 2048 `
        -HashAlgorithm SHA256 `
        -KeyExportPolicy NonExportable `
        -NotAfter (Get-Date).AddDays(2)
    $signed = Set-AuthenticodeSignature `
        -LiteralPath $fixture `
        -Certificate $certificate `
        -HashAlgorithm SHA256
    if (-not $signed.SignerCertificate) {
        throw "Self-test fixture signing failed: $($signed.StatusMessage)"
    }

    $validReport = Join-Path $root "valid-report.json"
    & (Join-Path $ProjectRoot `
        "tools\release\signing\verification\verify_authenticode_signature.ps1") `
        -ArtifactPath $fixture `
        -ReportPath $validReport `
        -ExpectedThumbprint $certificate.Thumbprint `
        -AllowUntrustedChain `
        -AllowMissingTimestamp
    $validResult = Read-KivoJson -Path $validReport
    if (-not $validResult.verified -or
        -not $validResult.artifacts[0].verified) {
        throw "Valid Authenticode report does not prove verification."
    }

    [System.IO.File]::AppendAllText(
        $fixture,
        "# tampered`r`n",
        [System.Text.UTF8Encoding]::new($false))
    $tamperedReport = Join-Path $root "tampered-report.json"
    $previousErrorAction = $ErrorActionPreference
    $ErrorActionPreference = "Continue"
    $failureOutput = & powershell -NoProfile -ExecutionPolicy Bypass -File `
        (Join-Path $ProjectRoot `
            "tools\release\signing\verification\verify_authenticode_signature.ps1") `
        -ArtifactPath $fixture `
        -ReportPath $tamperedReport `
        -ExpectedThumbprint $certificate.Thumbprint `
        -AllowUntrustedChain `
        -AllowMissingTimestamp 2>&1
    $failureExitCode = $LASTEXITCODE
    $ErrorActionPreference = $previousErrorAction
    if ($failureExitCode -eq 0) {
        throw "Tampered Authenticode fixture was accepted."
    }
    $tamperedResult = Read-KivoJson -Path $tamperedReport
    if ($tamperedResult.verified) {
        throw "Tampered Authenticode report incorrectly claims verification."
    }
} finally {
    if ($certificate) {
        Remove-Item -LiteralPath `
            "Cert:\CurrentUser\My\$($certificate.Thumbprint)" `
            -Force -ErrorAction SilentlyContinue
    }
}

Write-Host "AUTHENTICODE_VERIFIER_SELFTEST_PASS"
exit 0
