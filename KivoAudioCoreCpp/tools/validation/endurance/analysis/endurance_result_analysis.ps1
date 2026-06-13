Set-StrictMode -Version Latest

function Get-KivoLastLogMetric {
    param(
        [Parameter(Mandatory = $true)][string]$LogText,
        [Parameter(Mandatory = $true)][string]$Name
    )

    $pattern = "(?m)^\s*" + [regex]::Escape($Name) + "=(\d+)\s*$"
    $matches = [regex]::Matches($LogText, $pattern)
    if ($matches.Count -eq 0) {
        return $null
    }
    return [uint64]$matches[$matches.Count - 1].Groups[1].Value
}

function Get-KivoEnduranceClassification {
    param(
        [Parameter(Mandatory = $true)][int]$ExitCode,
        [Parameter(Mandatory = $true)][int]$RequestedHours,
        [Parameter(Mandatory = $true)][bool]$RuntimeCountersPassed,
        [Parameter(Mandatory = $true)][bool]$ResourceGrowthPassed,
        [Parameter(Mandatory = $true)][bool]$DeviceEvidenceAttached
    )

    if ($ExitCode -ne 0) {
        return "FAILED_PROCESS"
    }
    if (-not $RuntimeCountersPassed) {
        return "FAILED_RUNTIME_COUNTERS"
    }
    if (-not $ResourceGrowthPassed) {
        return "FAILED_RESOURCE_GROWTH"
    }
    if ($RequestedHours -lt 10) {
        return "DEVELOPMENT_EVIDENCE_ONLY"
    }
    if (-not $DeviceEvidenceAttached) {
        return "TEN_HOUR_SOFTWARE_RUN_PASSED_REQUIRES_DEVICE_RECORD_REVIEW"
    }
    return "TEN_HOUR_ENDURANCE_EVIDENCE_READY_FOR_REVIEW"
}
