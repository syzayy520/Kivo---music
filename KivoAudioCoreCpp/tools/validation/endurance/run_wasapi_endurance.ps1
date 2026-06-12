param(
    [Parameter(Mandatory = $true)][string]$ToneSmokePath,
    [ValidateRange(1, 20)][int]$Hours = 10,
    [ValidateSet("shared", "exclusive")][string]$Mode = "shared",
    [string]$OutputDirectory = "out\validation\endurance"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..\..")).Path
. (Join-Path $ProjectRoot "tools\release\foundation\release_foundation.ps1")

$executable = (Resolve-Path -LiteralPath $ToneSmokePath).Path
if ([System.IO.Path]::GetExtension($executable) -ne ".exe") {
    throw "ToneSmokePath must identify the hardware smoke executable."
}

$outRoot = Join-Path $ProjectRoot "out"
New-Item -ItemType Directory -Path $outRoot -Force | Out-Null
$output = Assert-KivoPathInside `
    -Path (Join-Path $ProjectRoot $OutputDirectory) `
    -Parent $outRoot
New-Item -ItemType Directory -Path $output -Force | Out-Null

$started = [DateTimeOffset]::UtcNow
$runId = $started.ToString("yyyyMMddTHHmmssZ")
$logPath = Join-Path $output "wasapi-endurance-$runId.log"
$evidencePath = Join-Path $output "wasapi-endurance-$runId.json"

$arguments = @(
    "--duration", "3600",
    "--cycles", $Hours.ToString(),
    "--mode", $Mode
)

$lines = @(
    "Kivo WASAPI endurance run"
    "started_utc=$($started.ToString('O'))"
    "hours=$Hours"
    "mode=$Mode"
    "executable_sha256=$(Get-KivoSha256 -Path $executable)"
)
$lines | Set-Content -LiteralPath $logPath -Encoding utf8

& $executable @arguments 2>&1 |
    Tee-Object -FilePath $logPath -Append
$exitCode = $LASTEXITCODE
$completed = [DateTimeOffset]::UtcNow

$evidence = [ordered]@{
    schema = "kivo.wasapi-endurance-evidence.v1"
    started_utc = $started.ToString("O")
    completed_utc = $completed.ToString("O")
    requested_hours = $Hours
    elapsed_seconds = [math]::Round(
        ($completed - $started).TotalSeconds,
        3)
    mode = $Mode
    executable = [ordered]@{
        file = Split-Path -Leaf $executable
        sha256 = Get-KivoSha256 -Path $executable
    }
    exit_code = $exitCode
    log = Split-Path -Leaf $logPath
    log_sha256 = Get-KivoSha256 -Path $logPath
    release_duration_met = $Hours -ge 10
    classification = if ($exitCode -ne 0) {
        "FAILED"
    } elseif ($Hours -lt 10) {
        "DEVELOPMENT_EVIDENCE_ONLY"
    } else {
        "TEN_HOUR_SOFTWARE_RUN_PASSED_REQUIRES_DEVICE_RECORD_REVIEW"
    }
}
Write-KivoJson -Path $evidencePath -Value $evidence

Write-Host "ENDURANCE_EVIDENCE: $evidencePath"
Write-Host "CLASSIFICATION: $($evidence.classification)"
if ($exitCode -ne 0) {
    exit $exitCode
}
