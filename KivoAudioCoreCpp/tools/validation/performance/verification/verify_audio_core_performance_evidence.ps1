param(
    [Parameter(Mandatory = $true)]
    [string]$EvidencePath
)

$ErrorActionPreference = "Stop"
$EvidencePath = (Resolve-Path -LiteralPath $EvidencePath).Path
$raw = Get-Content -LiteralPath $EvidencePath -Raw -Encoding UTF8
$document = $raw | ConvertFrom-Json
$violations = [Collections.Generic.List[string]]::new()

function Add-Violation {
    param([string]$Message)
    $violations.Add($Message)
}

function Get-Distribution {
    param([object[]]$Values)
    $sorted = @($Values | ForEach-Object { [uint64]$_ } | Sort-Object)
    if ($sorted.Count -eq 0) {
        return $null
    }
    $p50Index = [Math]::Ceiling($sorted.Count * 0.50) - 1
    $p95Index = [Math]::Ceiling($sorted.Count * 0.95) - 1
    return [ordered]@{
        minimum = $sorted[0]
        p50 = $sorted[$p50Index]
        p95 = $sorted[$p95Index]
        maximum = $sorted[-1]
    }
}

function Test-Distribution {
    param(
        [string]$Name,
        [object[]]$Values,
        [object]$Reported
    )
    $expected = Get-Distribution -Values $Values
    if ($null -eq $expected) {
        Add-Violation "$Name has no raw samples."
        return
    }
    foreach ($field in @("minimum", "p50", "p95", "maximum")) {
        if ([uint64]$Reported.$field -ne [uint64]$expected.$field) {
            Add-Violation "$Name $field does not match raw samples."
        }
    }
}

$expectedFixtures = [ordered]@{
    "tone_pcm_s16le.wav" =
        "101b74ef127db194060ca1240e16551f92d3449d75f0248c3894963290f53de1"
    "tone.flac" =
        "0239214dc0f0a21376185d71a482f4f0b1b704dc315a5432d2acb2d23783ce46"
    "tone.mp3" =
        "a111e9605c587b55f99c43998d86220ea234c15c12b6b31ed29b015a8a744a42"
    "tone_aac.m4a" =
        "32133a19bf3f4f5ced41dc2593511ccd48d97f4ca83aeaa23586f8ae68fc350e"
    "tone_alac.m4a" =
        "df10895e2083b127b80e6969e796ad786fab1dc21cca8ecbd9c519eb079a03c0"
    "tone_vorbis.ogg" =
        "0c71234421377f0d422a7e7253b433b8d5ca550bd3b55c24b65683bafe2187aa"
    "tone_opus.ogg" =
        "2f19be3f8d8592edecbc6ba1e9387999abe5bdd6e2a76c456a2e1f8285e664b4"
}

if ([uint32]$document.schema_version -ne 1) {
    Add-Violation "Unsupported schema version."
}
if ([uint32]$document.iterations -lt 5) {
    Add-Violation "At least five measured iterations are required."
}
if ($raw -match '(?i)[a-z]:\\\\|\\\\Users\\\\') {
    Add-Violation "Evidence contains an absolute Windows path."
}

$fixtures = @($document.fixtures)
if ($fixtures.Count -ne $expectedFixtures.Count) {
    Add-Violation "Fixture measurement count is incomplete."
}
foreach ($fixtureName in $expectedFixtures.Keys) {
    $fixture = @($fixtures | Where-Object fixture_name -eq $fixtureName)
    if ($fixture.Count -ne 1) {
        Add-Violation "Fixture measurement missing or duplicated: $fixtureName"
        continue
    }
    $samples = @($fixture[0].samples)
    if ($samples.Count -ne [uint32]$document.iterations) {
        Add-Violation "Fixture sample count mismatch: $fixtureName"
        continue
    }
    foreach ($sample in $samples) {
        foreach ($metric in @(
            "open_ns",
            "first_block_ns",
            "seek_ns",
            "full_decode_ns",
            "decoded_frames",
            "decoded_blocks")) {
            if ([uint64]$sample.$metric -eq 0) {
                Add-Violation "$fixtureName contains zero $metric."
            }
        }
    }
    $reported = $fixture[0].distributions_ns
    Test-Distribution "$fixtureName open" `
        @($samples.open_ns) $reported.open
    Test-Distribution "$fixtureName first_block" `
        @($samples.first_block_ns) $reported.first_block
    Test-Distribution "$fixtureName seek" `
        @($samples.seek_ns) $reported.seek
    Test-Distribution "$fixtureName full_decode" `
        @($samples.full_decode_ns) $reported.full_decode
}

$queue = $document.queue
$occupancy = @($queue.occupancy_samples)
$intervals = @($queue.consumer_interval_ns)
if ([uint32]$queue.capacity_slots -eq 0) {
    Add-Violation "Queue capacity is zero."
}
if ([uint32]$queue.observed_peak_used_slots -gt
    [uint32]$queue.capacity_slots) {
    Add-Violation "Queue observed peak exceeds capacity."
}
if ($occupancy.Count -eq 0 -or $intervals.Count -ne $occupancy.Count - 1) {
    Add-Violation "Queue timing sample counts are inconsistent."
}
if ([uint64]$queue.successful_pushes -ne $occupancy.Count -or
    [uint64]$queue.successful_consumes -ne $occupancy.Count) {
    Add-Violation "Queue operation counts do not match samples."
}
foreach ($value in $occupancy) {
    if ([uint32]$value -gt [uint32]$queue.capacity_slots) {
        Add-Violation "Queue occupancy sample exceeds capacity."
        break
    }
}
if (@($intervals | Where-Object { [uint64]$_ -eq 0 }).Count -ne 0) {
    Add-Violation "Queue interval contains a zero sample."
}
Test-Distribution "queue consumer interval" $intervals `
    $queue.consumer_interval_distribution_ns

$provenance = $document.provenance
if ($provenance.source_revision -notmatch '^[0-9a-f]{40}$') {
    Add-Violation "Source revision is not a full Git commit."
}
if ($provenance.probe_sha256 -notmatch '^[0-9a-f]{64}$') {
    Add-Violation "Probe SHA-256 is invalid."
}
$captured = [DateTime]::MinValue
if (-not [DateTime]::TryParse(
    [string]$provenance.captured_utc,
    [ref]$captured)) {
    Add-Violation "Capture timestamp is invalid."
}
$provenanceFixtures = @($provenance.fixtures)
if ($provenanceFixtures.Count -ne $expectedFixtures.Count) {
    Add-Violation "Fixture provenance count is incomplete."
}
foreach ($fixtureName in $expectedFixtures.Keys) {
    $record = @(
        $provenanceFixtures | Where-Object fixture_name -eq $fixtureName)
    if ($record.Count -ne 1) {
        Add-Violation "Fixture provenance missing or duplicated: $fixtureName"
        continue
    }
    if ($record[0].sha256 -ne $expectedFixtures[$fixtureName]) {
        Add-Violation "Fixture hash mismatch: $fixtureName"
    }
    if ([uint64]$record[0].size_bytes -eq 0) {
        Add-Violation "Fixture size is zero: $fixtureName"
    }
}

if ($violations.Count -ne 0) {
    Write-Host "FAIL: performance evidence rejected"
    foreach ($violation in $violations) {
        Write-Host "  - $violation"
    }
    exit 1
}

Write-Host "PASS: performance evidence is structurally valid"
Write-Host "Working tree clean at capture: $($provenance.working_tree_clean)"
exit 0
