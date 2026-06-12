param(
    [Parameter(Mandatory = $true)]
    [ValidateSet("Clean", "Upgrade", "Rollback")]
    [string]$Mode,
    [Parameter(Mandatory = $true)][string]$CandidateRuntimeDirectory,
    [Parameter(Mandatory = $true)][string]$InstalledRuntimeDirectory,
    [string]$PreviousRuntimeDirectory,
    [Parameter(Mandatory = $true)][string]$ReportPath,
    [switch]$RequireSignedCore
)

$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..\..\..")).Path
. (Join-Path $ProjectRoot `
    "tools\release\foundation\release_foundation.ps1")
. (Join-Path $ProjectRoot `
    "tools\release\installer\foundation\installer_payload_snapshot.ps1")

$errors = [System.Collections.Generic.List[string]]::new()
$candidate = Get-KivoInstallerPayloadSnapshot `
    -RuntimeDirectory $CandidateRuntimeDirectory
$installed = Get-KivoInstallerPayloadSnapshot `
    -RuntimeDirectory $InstalledRuntimeDirectory
$previous = $null

if ($Mode -in @("Upgrade", "Rollback")) {
    if (-not $PreviousRuntimeDirectory) {
        $errors.Add("$Mode verification requires PreviousRuntimeDirectory.")
    } else {
        $previous = Get-KivoInstallerPayloadSnapshot `
            -RuntimeDirectory $PreviousRuntimeDirectory
    }
}

$expected = switch ($Mode) {
    "Clean" { $candidate }
    "Upgrade" { $candidate }
    "Rollback" { $previous }
}
if ($null -ne $expected) {
    foreach ($error in Compare-KivoPayloadSnapshot `
        -Expected $expected `
        -Actual $installed `
        -Label $Mode) {
        $errors.Add($error)
    }
}

if ($null -ne $previous) {
    if ($candidate.Fingerprint -eq $previous.Fingerprint) {
        $errors.Add(
            "$Mode evidence requires distinct candidate and previous payloads.")
    }
    if ($candidate.SourceCommit -eq $previous.SourceCommit) {
        $errors.Add(
            "$Mode evidence requires distinct source commits.")
    }
}

if ($RequireSignedCore) {
    if (-not $installed.ManifestSigned) {
        $errors.Add("Installed manifest does not declare a signed core.")
    }
    if ($installed.CoreSignatureStatus -ne "Valid") {
        $errors.Add(
            "Installed core Authenticode status is " +
            "$($installed.CoreSignatureStatus), expected Valid.")
    }
}

$report = [ordered]@{
    schema = "kivo.installer-transaction-verification.v1"
    generated_at = (Get-Date).ToUniversalTime().ToString("o")
    mode = $Mode.ToLowerInvariant()
    passed = $errors.Count -eq 0
    signed_core_required = [bool]$RequireSignedCore
    errors = @($errors)
    candidate = [ordered]@{
        version = $candidate.Version
        source_commit = $candidate.SourceCommit
        fingerprint = $candidate.Fingerprint
        valid = $candidate.Valid
    }
    previous = if ($null -eq $previous) {
        $null
    } else {
        [ordered]@{
            version = $previous.Version
            source_commit = $previous.SourceCommit
            fingerprint = $previous.Fingerprint
            valid = $previous.Valid
        }
    }
    installed = [ordered]@{
        version = $installed.Version
        source_commit = $installed.SourceCommit
        fingerprint = $installed.Fingerprint
        valid = $installed.Valid
        manifest_signed = $installed.ManifestSigned
        core_signature_status = $installed.CoreSignatureStatus
    }
}
Write-KivoJson -Path $ReportPath -Value $report

if ($errors.Count -ne 0) {
    throw "Installer transaction verification failed: $($errors -join '; ')"
}
Write-Host "INSTALLER_TRANSACTION_VERIFICATION_PASS: $Mode"
