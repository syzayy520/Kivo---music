param(
    [Parameter(Mandatory = $true)][string]$ToneSmokePath,
    [ValidateRange(1, 20)][int]$Hours = 10,
    [ValidateSet("shared", "exclusive")][string]$Mode = "shared",
    [string]$OutputDirectory = "out\validation\endurance",
    [ValidateRange(5, 600)][int]$SampleIntervalSeconds = 60,
    [ValidateRange(0, 3600)][int]$WarmupSeconds = 120,
    [ValidateRange(0, [int64]::MaxValue)]
    [int64]$MaximumPrivateBytesGrowth = 134217728,
    [ValidateRange(0, [int]::MaxValue)]
    [int]$MaximumHandleGrowth = 64,
    [ValidateRange(0, [int]::MaxValue)]
    [int]$MaximumThreadGrowth = 8,
    [ValidateRange(0, [int]::MaxValue)]
    [int]$MaximumGdiObjectGrowth = 16,
    [ValidateRange(0, [int]::MaxValue)]
    [int]$MaximumUserObjectGrowth = 16,
    [string]$DeviceEvidencePath,
    [string]$DeviceRowId
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..\..")).Path
. (Join-Path $ProjectRoot "tools\release\foundation\release_foundation.ps1")
. (Join-Path $PSScriptRoot `
    "foundation\process_resource_sampler.ps1")
. (Join-Path $PSScriptRoot `
    "analysis\endurance_result_analysis.ps1")

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
$resourcePath = Join-Path $output "wasapi-endurance-$runId-resources.csv"
$stdoutPath = Join-Path $output "wasapi-endurance-$runId.stdout.log"
$stderrPath = Join-Path $output "wasapi-endurance-$runId.stderr.log"

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
[System.IO.File]::WriteAllLines(
    $logPath,
    $lines,
    [System.Text.UTF8Encoding]::new($false))

$process = Start-Process `
    -FilePath $executable `
    -ArgumentList $arguments `
    -RedirectStandardOutput $stdoutPath `
    -RedirectStandardError $stderrPath `
    -WindowStyle Hidden `
    -PassThru
$samples = [System.Collections.Generic.List[object]]::new()
while (-not $process.HasExited) {
    try {
        $samples.Add(
            (Get-KivoProcessResourceSample `
                -Process $process `
                -Started $started))
    } catch {
        Write-Warning "Resource sample failed: $($_.Exception.Message)"
    }
    Start-Sleep -Seconds $SampleIntervalSeconds
}
$process.WaitForExit()
$exitCode = $process.ExitCode
$completed = [DateTimeOffset]::UtcNow
if (Test-Path -LiteralPath $stdoutPath) {
    [System.IO.File]::AppendAllText(
        $logPath,
        [Environment]::NewLine +
            [System.IO.File]::ReadAllText($stdoutPath))
}
if (Test-Path -LiteralPath $stderrPath) {
    [System.IO.File]::AppendAllText(
        $logPath,
        [Environment]::NewLine +
            [System.IO.File]::ReadAllText($stderrPath))
}
Remove-Item -LiteralPath $stdoutPath,$stderrPath -Force
@($samples) | Export-Csv `
    -LiteralPath $resourcePath `
    -NoTypeInformation `
    -Encoding UTF8

$resourceSummary = Get-KivoResourceSummary `
    -Samples @($samples) `
    -WarmupSeconds $WarmupSeconds
$resourceGrowthPassed = $null -ne $resourceSummary
if ($resourceGrowthPassed) {
    $resourceGrowthPassed =
        $resourceSummary.growth.private_bytes `
            -le $MaximumPrivateBytesGrowth -and
        $resourceSummary.growth.handle_count `
            -le $MaximumHandleGrowth -and
        $resourceSummary.growth.thread_count `
            -le $MaximumThreadGrowth -and
        $resourceSummary.growth.gdi_objects `
            -le $MaximumGdiObjectGrowth -and
        $resourceSummary.growth.user_objects `
            -le $MaximumUserObjectGrowth
}

$logText = Get-Content -Raw -LiteralPath $logPath
$runtimeCounters = [ordered]@{
    worker_wait_timeouts =
        Get-KivoLastLogMetric -LogText $logText -Name "worker_wait_timeouts"
    underrun_count =
        Get-KivoLastLogMetric -LogText $logText -Name "underrun_count"
    overrun_count =
        Get-KivoLastLogMetric -LogText $logText -Name "overrun_count"
    device_invalidations =
        Get-KivoLastLogMetric -LogText $logText -Name "device_invalidations"
}
$runtimeCountersPassed = $true
foreach ($value in $runtimeCounters.Values) {
    if ($null -eq $value -or $value -ne 0) {
        $runtimeCountersPassed = $false
    }
}

$deviceEvidence = $null
if ($DeviceEvidencePath) {
    if (-not $DeviceRowId) {
        throw "DeviceRowId is required with DeviceEvidencePath."
    }
    $resolvedDeviceEvidence =
        (Resolve-Path -LiteralPath $DeviceEvidencePath).Path
    $deviceRecord = Get-Content -Raw -Encoding UTF8 `
        -LiteralPath $resolvedDeviceEvidence |
        ConvertFrom-Json
    if ($deviceRecord.schema -ne "kivo.device-matrix-evidence.v1") {
        throw "Device evidence schema is unsupported."
    }
    $matchingRows = @(
        $deviceRecord.rows |
            Where-Object { $_.id -eq $DeviceRowId })
    if ($matchingRows.Count -ne 1 -or
        $matchingRows[0].status -ne "pass" -or
        -not $matchingRows[0].evidence) {
        throw "Device row is not a completed physical PASS: $DeviceRowId"
    }
    $deviceEvidence = [ordered]@{
        file = Split-Path -Leaf $resolvedDeviceEvidence
        sha256 = Get-KivoSha256 -Path $resolvedDeviceEvidence
        machine_id = [string]$deviceRecord.machine_id
        row_id = $DeviceRowId
        row_evidence = [string]$matchingRows[0].evidence
    }
}
$classification = Get-KivoEnduranceClassification `
    -ExitCode $exitCode `
    -RequestedHours $Hours `
    -RuntimeCountersPassed $runtimeCountersPassed `
    -ResourceGrowthPassed $resourceGrowthPassed `
    -DeviceEvidenceAttached ($null -ne $deviceEvidence)

$evidence = [ordered]@{
    schema = "kivo.wasapi-endurance-evidence.v2"
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
    resources = [ordered]@{
        file = Split-Path -Leaf $resourcePath
        sha256 = Get-KivoSha256 -Path $resourcePath
        sample_interval_seconds = $SampleIntervalSeconds
        warmup_seconds = $WarmupSeconds
        thresholds = [ordered]@{
            maximum_private_bytes_growth =
                $MaximumPrivateBytesGrowth
            maximum_handle_growth = $MaximumHandleGrowth
            maximum_thread_growth = $MaximumThreadGrowth
            maximum_gdi_object_growth = $MaximumGdiObjectGrowth
            maximum_user_object_growth = $MaximumUserObjectGrowth
        }
        summary = $resourceSummary
        growth_passed = $resourceGrowthPassed
    }
    runtime_counters = $runtimeCounters
    runtime_counters_passed = $runtimeCountersPassed
    device_evidence = $deviceEvidence
    release_duration_met = $Hours -ge 10
    classification = $classification
}
Write-KivoJson -Path $evidencePath -Value $evidence

Write-Host "ENDURANCE_EVIDENCE: $evidencePath"
Write-Host "CLASSIFICATION: $($evidence.classification)"
if ($exitCode -ne 0) {
    exit $exitCode
}
if (-not $runtimeCountersPassed -or -not $resourceGrowthPassed) {
    exit 14
}
