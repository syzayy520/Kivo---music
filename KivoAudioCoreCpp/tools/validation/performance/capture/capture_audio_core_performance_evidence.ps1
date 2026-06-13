param(
    [Parameter(Mandatory = $true)]
    [string]$ProbePath,

    [Parameter(Mandatory = $true)]
    [string]$FixtureDirectory,

    [Parameter(Mandatory = $true)]
    [string]$OutputPath,

    [ValidateRange(1, 100)]
    [int]$Iterations = 5,

    [string]$ProjectRoot,

    [string]$Configuration = "Release"
)

$ErrorActionPreference = "Stop"

if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\..\..\.."
}
$ProjectRoot = (Resolve-Path -LiteralPath $ProjectRoot).Path
$ProbePath = (Resolve-Path -LiteralPath $ProbePath).Path
$FixtureDirectory = (Resolve-Path -LiteralPath $FixtureDirectory).Path
$OutputPath = [IO.Path]::GetFullPath($OutputPath)

if (-not (Test-Path -LiteralPath $ProbePath -PathType Leaf)) {
    throw "Performance probe is not a file."
}
if (-not (Test-Path -LiteralPath $FixtureDirectory -PathType Container)) {
    throw "Performance fixture directory is missing."
}

$outputDirectory = Split-Path -Parent $OutputPath
if ($outputDirectory) {
    [IO.Directory]::CreateDirectory($outputDirectory) | Out-Null
}

& $ProbePath $FixtureDirectory $OutputPath $Iterations
if ($LASTEXITCODE -ne 0) {
    throw "Performance probe failed with exit code $LASTEXITCODE."
}

$document = Get-Content -LiteralPath $OutputPath -Raw -Encoding UTF8 |
    ConvertFrom-Json
$fixtureRecords = @(
    foreach ($fixture in $document.fixtures) {
        $fixturePath = Join-Path $FixtureDirectory $fixture.fixture_name
        if (-not (Test-Path -LiteralPath $fixturePath -PathType Leaf)) {
            throw "Measured fixture is missing: $($fixture.fixture_name)"
        }
        $item = Get-Item -LiteralPath $fixturePath
        [ordered]@{
            fixture_name = $fixture.fixture_name
            size_bytes = [uint64]$item.Length
            sha256 = (Get-FileHash -LiteralPath $fixturePath -Algorithm SHA256).
                Hash.ToLowerInvariant()
        }
    }
)

$sourceRevision = (& git -C $ProjectRoot rev-parse HEAD).Trim()
if ($LASTEXITCODE -ne 0) {
    throw "Unable to resolve source revision."
}
$workingTreeStatus = @(& git -C $ProjectRoot status --porcelain)
if ($LASTEXITCODE -ne 0) {
    throw "Unable to inspect working tree status."
}

$provenance = [ordered]@{
    captured_utc = [DateTime]::UtcNow.ToString("o")
    source_revision = $sourceRevision
    working_tree_clean = $workingTreeStatus.Count -eq 0
    probe_sha256 = (Get-FileHash -LiteralPath $ProbePath -Algorithm SHA256).
        Hash.ToLowerInvariant()
    configuration = $Configuration
    environment = [ordered]@{
        os = [Runtime.InteropServices.RuntimeInformation]::OSDescription
        architecture =
            [Runtime.InteropServices.RuntimeInformation]::OSArchitecture.ToString()
        processor_count = [Environment]::ProcessorCount
        processor_identifier = [Environment]::GetEnvironmentVariable(
            "PROCESSOR_IDENTIFIER")
    }
    fixtures = $fixtureRecords
}

$document | Add-Member -NotePropertyName provenance -NotePropertyValue $provenance
$json = $document | ConvertTo-Json -Depth 20
[IO.File]::WriteAllText(
    $OutputPath,
    $json + [Environment]::NewLine,
    [Text.UTF8Encoding]::new($false))

$verifier = Join-Path $PSScriptRoot `
    "..\verification\verify_audio_core_performance_evidence.ps1"
& $verifier -EvidencePath $OutputPath
if ($LASTEXITCODE -ne 0) {
    throw "Performance evidence verification failed."
}

Write-Host "Performance evidence captured: $OutputPath"
Write-Host "Source revision: $sourceRevision"
Write-Host "Working tree clean: $($workingTreeStatus.Count -eq 0)"
