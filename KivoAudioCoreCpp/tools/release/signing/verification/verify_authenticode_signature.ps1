param(
    [Parameter(Mandatory = $true)][string[]]$ArtifactPath,
    [Parameter(Mandatory = $true)][string]$ReportPath,
    [string]$ExpectedThumbprint,
    [switch]$AllowUntrustedChain,
    [switch]$AllowMissingTimestamp
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..\..\..")).Path
. (Join-Path $ProjectRoot "tools\release\foundation\release_foundation.ps1")
. (Join-Path $ProjectRoot "tools\validation\import_msvc_environment.ps1")
. (Join-Path $PSScriptRoot "..\foundation\authenticode_foundation.ps1")

$records = [System.Collections.Generic.List[object]]::new()
$errors = [System.Collections.Generic.List[string]]::new()
$expected = if ($ExpectedThumbprint) {
    $ExpectedThumbprint.Replace(" ", "").ToUpperInvariant()
} else {
    ""
}

foreach ($requestedPath in $ArtifactPath) {
    $resolved = (Resolve-Path -LiteralPath $requestedPath).Path
    $signature = Get-AuthenticodeSignature -LiteralPath $resolved
    $artifactErrors = [System.Collections.Generic.List[string]]::new()
    $certificate = $signature.SignerCertificate

    if (-not $certificate) {
        $artifactErrors.Add("Artifact has no Authenticode signer certificate.")
    } else {
        if ($expected -and $certificate.Thumbprint -ne $expected) {
            $artifactErrors.Add("Signer thumbprint differs from expected value.")
        }
        if (-not (Test-KivoCodeSigningEku -Certificate $certificate)) {
            $artifactErrors.Add("Signer certificate lacks code-signing EKU.")
        }
    }

    $status = [string]$signature.Status
    if ($AllowUntrustedChain) {
        if (-not (Test-KivoAllowedTestSignatureStatus -Status $status)) {
            $artifactErrors.Add("Signature status is $status.")
        }
    } elseif ($status -ne "Valid") {
        $artifactErrors.Add("Signature trust status is $status.")
    }

    $hasTimestamp = $null -ne $signature.TimeStamperCertificate
    if (-not $AllowMissingTimestamp -and -not $hasTimestamp) {
        $artifactErrors.Add("RFC 3161 timestamp evidence is missing.")
    }

    $signtoolVerified = $false
    if (-not $AllowUntrustedChain) {
        if (-not (Import-KivoMsvcEnvironment)) {
            $artifactErrors.Add("Windows SDK verification environment is unavailable.")
        } else {
            [Environment]::SetEnvironmentVariable("VSLANG", "1033", "Process")
            $signtoolOutput = & signtool.exe verify /pa /all /v $resolved 2>&1
            $signtoolVerified = $LASTEXITCODE -eq 0
            if (-not $signtoolVerified) {
                $artifactErrors.Add("signtool trust verification failed.")
            } elseif (($signtoolOutput -join "`n") -notmatch
                "(?i)Hash of file \(sha256\)") {
                $artifactErrors.Add("signtool did not report a SHA-256 file hash.")
            }
        }
    }

    foreach ($errorMessage in $artifactErrors) {
        $errors.Add("$(Split-Path -Leaf $resolved): $errorMessage")
    }
    $records.Add([ordered]@{
        artifact = Split-Path -Leaf $resolved
        size = (Get-Item -LiteralPath $resolved).Length
        sha256 = Get-KivoSha256 -Path $resolved
        signature_status = $status
        signature_type = [string]$signature.SignatureType
        trusted_chain_required = -not $AllowUntrustedChain
        signtool_verified = $signtoolVerified
        has_timestamp = $hasTimestamp
        signer = if ($certificate) {
            Get-KivoCertificateRecord -Certificate $certificate
        } else {
            $null
        }
        timestamper = if ($signature.TimeStamperCertificate) {
            Get-KivoCertificateRecord `
                -Certificate $signature.TimeStamperCertificate
        } else {
            $null
        }
        verified = $artifactErrors.Count -eq 0
    })
}

$report = [ordered]@{
    schema = "kivo.authenticode-verification.v1"
    verified = $errors.Count -eq 0
    require_trusted_chain = -not $AllowUntrustedChain
    require_timestamp = -not $AllowMissingTimestamp
    expected_thumbprint = $expected
    artifacts = @($records)
    errors = @($errors)
}
Write-KivoJson -Path $ReportPath -Value $report

if ($errors.Count -ne 0) {
    throw "Authenticode verification failed: $($errors -join '; ')"
}
Write-Host "AUTHENTICODE_VERIFICATION_PASS"
