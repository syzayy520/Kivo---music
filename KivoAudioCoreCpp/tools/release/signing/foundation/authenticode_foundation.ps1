Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Test-KivoCodeSigningEku {
    param(
        [Parameter(Mandatory = $true)]
        [System.Security.Cryptography.X509Certificates.X509Certificate2]
        $Certificate
    )

    foreach ($extension in $Certificate.Extensions) {
        if ($extension -is
            [System.Security.Cryptography.X509Certificates.X509EnhancedKeyUsageExtension]) {
            foreach ($oid in $extension.EnhancedKeyUsages) {
                if ($oid.Value -eq "1.3.6.1.5.5.7.3.3") {
                    return $true
                }
            }
        }
    }
    return $false
}

function Get-KivoCertificateRecord {
    param(
        [Parameter(Mandatory = $true)]
        [System.Security.Cryptography.X509Certificates.X509Certificate2]
        $Certificate
    )

    return [ordered]@{
        thumbprint = $Certificate.Thumbprint
        subject = $Certificate.Subject
        issuer = $Certificate.Issuer
        serial_number = $Certificate.SerialNumber
        not_before = $Certificate.NotBefore.ToUniversalTime().ToString("o")
        not_after = $Certificate.NotAfter.ToUniversalTime().ToString("o")
        has_code_signing_eku = Test-KivoCodeSigningEku `
            -Certificate $Certificate
    }
}

function Test-KivoAllowedTestSignatureStatus {
    param([Parameter(Mandatory = $true)][string]$Status)

    return $Status -in @("Valid", "UnknownError", "NotTrusted")
}
