param(
    [Parameter(Mandatory = $true)][string]$EvidencePath,
    [switch]$RequireComplete
)

$ErrorActionPreference = "Stop"
$evidence = Get-Content -Raw -Encoding UTF8 -LiteralPath $EvidencePath |
    ConvertFrom-Json
$errors = [System.Collections.Generic.List[string]]::new()
$requiredIds = @(
    "integrated-shared",
    "usb-dac-class-1",
    "usb-dac-class-2",
    "bluetooth-a2dp-reconnect",
    "hdmi-active-display",
    "default-device-switch",
    "unplug-replug",
    "disable-enable",
    "driver-restart",
    "sleep-resume"
)

if ($evidence.schema -ne "kivo.device-matrix-evidence.v1") {
    $errors.Add("Unsupported evidence schema.")
}
if (-not $evidence.machine_id) {
    $errors.Add("Machine id is missing.")
}
if (-not $evidence.probe.inventory_valid) {
    $errors.Add("Endpoint inventory is invalid.")
}
foreach ($id in $requiredIds) {
    $rows = @($evidence.rows | Where-Object { $_.id -eq $id })
    if ($rows.Count -ne 1) {
        $errors.Add("Required row is missing or duplicated: $id")
        continue
    }
    $status = [string]$rows[0].status
    if ($status -notin @("pass", "fail", "not_run", "not_present")) {
        $errors.Add("Invalid status for ${id}: $status")
    }
    if ($RequireComplete -and $status -ne "pass") {
        $errors.Add("Physical PASS evidence is missing: $id ($status)")
    }
    if ($status -eq "pass" -and -not $rows[0].evidence) {
        $errors.Add("PASS row has no evidence reference: $id")
    }
}

if ($errors.Count -ne 0) {
    throw "Device matrix evidence failed: $($errors -join '; ')"
}
if ($RequireComplete) {
    Write-Host "DEVICE_MATRIX_COMPLETE"
} else {
    Write-Host "DEVICE_MATRIX_SCHEMA_VALID"
}
