param(
    [Parameter(Mandatory = $true)][string]$IssueId,
    [Parameter(Mandatory = $true)][string]$AffectedRelease,
    [Parameter(Mandatory = $true)]
    [ValidateSet("critical", "high", "medium", "low")]
    [string]$Severity,
    [Parameter(Mandatory = $true)][string]$Owner,
    [Parameter(Mandatory = $true)][string]$OutputPath
)

$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..\..")).Path
. (Join-Path $ProjectRoot `
    "tools\release\foundation\release_foundation.ps1")

$record = [ordered]@{
    schema = "kivo.commercial-audio-regression.v1"
    issue_id = $IssueId
    status = "intake"
    severity = $Severity
    owner = $Owner
    affected_release = $AffectedRelease
    environment = [ordered]@{
        os_build = $null
        device_class = $null
        driver_version = $null
    }
    sanitized_diagnostic_snapshot = [ordered]@{}
    reproduction = [ordered]@{
        classification = "not_reproduced"
        deterministic_test_id = $null
        hardware_evidence_sha256 = $null
        privacy_safe_steps = $null
    }
    resolution = [ordered]@{
        fix_commit = $null
        released_version = $null
        regression_evidence = $null
        support_closed_at = $null
    }
}
Write-KivoJson -Path $OutputPath -Value $record
Write-Host "COMMERCIAL_REGRESSION_RECORD_CREATED: $OutputPath"
