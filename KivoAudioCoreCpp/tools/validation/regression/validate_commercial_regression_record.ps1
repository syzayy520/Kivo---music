param(
    [Parameter(Mandatory = $true)][string]$RecordPath,
    [ValidateSet("Intake", "Closure")][string]$Stage = "Intake"
)

$ErrorActionPreference = "Stop"
$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..\..")).Path
. (Join-Path $ProjectRoot `
    "tools\validation\regression\foundation\commercial_regression_rules.ps1")

$record = Get-Content -Raw -Encoding UTF8 -LiteralPath $RecordPath |
    ConvertFrom-Json
$errors = [System.Collections.Generic.List[string]]::new()
if ($record.schema -ne "kivo.commercial-audio-regression.v1") {
    $errors.Add("Unsupported regression schema.")
}
foreach ($field in @(
    "issue_id",
    "severity",
    "owner",
    "affected_release")) {
    if (-not $record.$field) {
        $errors.Add("Required intake field is missing: $field")
    }
}
if ($record.severity -notin @("critical", "high", "medium", "low")) {
    $errors.Add("Severity is invalid.")
}
if (-not (Test-KivoNumericDiagnosticObject `
        -Value $record.sanitized_diagnostic_snapshot)) {
    $errors.Add(
        "Diagnostic snapshot must contain numeric, boolean, array, or null values only.")
}
$allText = $record | ConvertTo-Json -Depth 32
if (-not (Test-KivoRegressionTextPrivacy -Value $allText)) {
    $errors.Add("Regression record contains a forbidden private or secret pattern.")
}

if ($Stage -eq "Closure") {
    if ($record.status -ne "closed") {
        $errors.Add("Closure stage requires status=closed.")
    }
    if (-not $record.environment.os_build -or
        -not $record.environment.device_class) {
        $errors.Add("Closure requires OS build and device class.")
    }
    $reproduction = $record.reproduction
    if ($reproduction.classification -notin @(
            "deterministic_test",
            "physical_hardware")) {
        $errors.Add("Closure requires a durable reproduction classification.")
    }
    if ($reproduction.classification -eq "deterministic_test" -and
        -not $reproduction.deterministic_test_id) {
        $errors.Add("Deterministic closure requires a test id.")
    }
    if ($reproduction.classification -eq "physical_hardware" -and
        $reproduction.hardware_evidence_sha256 -notmatch
            "^[0-9a-fA-F]{64}$") {
        $errors.Add("Hardware closure requires an evidence SHA-256.")
    }
    if (-not (Test-KivoGitCommit `
            -Value $record.resolution.fix_commit)) {
        $errors.Add("Closure requires a full 40-character fix commit.")
    }
    if (-not $record.resolution.released_version -or
        -not $record.resolution.regression_evidence -or
        -not $record.resolution.support_closed_at) {
        $errors.Add(
            "Closure requires released version, regression evidence, and support close time.")
    }
}

if ($errors.Count -ne 0) {
    throw "Commercial regression record failed: $($errors -join '; ')"
}
Write-Host "COMMERCIAL_REGRESSION_RECORD_VALID: $Stage"
