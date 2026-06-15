param(
    [Parameter(Mandatory = $true)][string]$DeviceMatrixExecutable,
    [Parameter(Mandatory = $true)][string]$MachineId,
    [Parameter(Mandatory = $true)][string]$OutputDirectory
)

$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..\..")).Path
. (Join-Path $ProjectRoot `
    "tools\release\foundation\release_foundation.ps1")

$executable = (Resolve-Path -LiteralPath $DeviceMatrixExecutable).Path
$outputParent = Split-Path -Parent `
    ([System.IO.Path]::GetFullPath($OutputDirectory))
$output = Reset-KivoDirectory `
    -Path $OutputDirectory `
    -AllowedParent $outputParent
$inventoryPath = Join-Path $output "endpoint-inventory.json"
$markdownPath = Join-Path $output "endpoint-inventory.md"
$evidencePath = Join-Path $output "device-matrix-evidence.json"

$console = & $executable `
    --json $inventoryPath `
    --machine-id $MachineId 2>&1
$exitCode = $LASTEXITCODE
Write-KivoText `
    -Path $markdownPath `
    -Value (($console -join [Environment]::NewLine) + [Environment]::NewLine)
if ($exitCode -ne 0) {
    throw "Device inventory executable failed with exit code $exitCode."
}

$inventory = Get-Content -Raw -Encoding UTF8 -LiteralPath $inventoryPath |
    ConvertFrom-Json
$os = Get-CimInstance Win32_OperatingSystem
$system = Get-CimInstance Win32_ComputerSystem
$requiredRows = @(
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
) | ForEach-Object {
    [ordered]@{
        id = $_
        status = "not_run"
        evidence = $null
        notes = $null
    }
}

$evidence = [ordered]@{
    schema = "kivo.device-matrix-evidence.v1"
    captured_at = (Get-Date).ToUniversalTime().ToString("o")
    machine_id = $MachineId
    host = [ordered]@{
        os_caption = [string]$os.Caption
        os_version = [string]$os.Version
        os_build = [string]$os.BuildNumber
        architecture = [string]$os.OSArchitecture
        manufacturer = [string]$system.Manufacturer
        model = [string]$system.Model
    }
    probe = [ordered]@{
        executable_sha256 = Get-KivoSha256 -Path $executable
        inventory_sha256 = Get-KivoSha256 -Path $inventoryPath
        inventory_valid = [bool]$inventory.inventory_valid
        endpoint_count = [int]$inventory.endpoint_count
    }
    rows = @($requiredRows)
    release_matrix_complete = $false
    completion_rule =
        "Every required row needs physical PASS evidence; not_run, not_present, or fail is incomplete."
}
Write-KivoJson -Path $evidencePath -Value $evidence
Write-Host "DEVICE_MATRIX_EVIDENCE_CAPTURED: $evidencePath"
