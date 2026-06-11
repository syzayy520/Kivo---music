# =============================================================================
# check_dependency_license_gate.ps1
# Dependency / License Gate - Controlled External Dependency Validation
# =============================================================================

param(
    [string]$ProjectRoot
)

$ErrorActionPreference = "Stop"

if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
}
$ProjectRoot = (Resolve-Path $ProjectRoot).Path

Write-Host "=== Dependency / License Gate ==="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host ""

$violations = @()

foreach ($dir in @("vendor", "third_party", "bin", "lib")) {
    if (Test-Path (Join-Path $ProjectRoot $dir)) {
        $violations += "DEPENDENCY_DIR: $dir/ exists"
    }
}

$cmakePath = Join-Path $ProjectRoot "CMakeLists.txt"
if (-not (Test-Path $cmakePath)) {
    $violations += "CMAKE: CMakeLists.txt is missing"
} else {
    $cmakeContent = Get-Content $cmakePath -Raw
    foreach ($downloadToken in @(
        "FetchContent",
        "file(DOWNLOAD",
        "ExternalProject_Add",
        "Invoke-WebRequest",
        "Start-BitsTransfer",
        "curl ",
        "wget "
    )) {
        if ($cmakeContent -match [regex]::Escape($downloadToken)) {
            $violations += "DOWNLOAD_MECHANISM: CMakeLists.txt contains $downloadToken"
        }
    }
    $lines = $cmakeContent -split "`n"
    foreach ($line in $lines) {
        if ($line -match "find_package" -and $line -notmatch "^\s*#") {
            $violations += "FIND_PACKAGE: CMakeLists.txt contains uncommented find_package"
            break
        }
    }
    if ($cmakeContent -notmatch "KIVO_FFMPEG_ROOT") {
        $violations += "FFMPEG_ROOT: approved external SDK root is not declared"
    }
}

foreach ($manifest in @("vcpkg.json", "conanfile.txt", "conanfile.py")) {
    if (Test-Path (Join-Path $ProjectRoot $manifest)) {
        $violations += "PACKAGE_MANAGER: $manifest exists"
    }
}
if (Test-Path (Join-Path $ProjectRoot ".gitmodules")) {
    $violations += "SUBMODULE: .gitmodules exists"
}

$presetPath = Join-Path $ProjectRoot "CMakePresets.json"
if (Test-Path $presetPath) {
    $presetContent = Get-Content $presetPath -Raw
    if ($presetContent -match "toolchainFile") {
        $violations += "TOOLCHAIN: CMakePresets.json references toolchainFile"
    }
}

$ffmpegManifestPath =
    Join-Path $ProjectRoot "docs\dependencies\ffmpeg_lgpl_shared_7_1.md"
if (-not (Test-Path $ffmpegManifestPath)) {
    $violations += "FFMPEG_PROVENANCE: baseline manifest is missing"
} else {
    $manifest = Get-Content $ffmpegManifestPath -Raw
    $requiredEvidence = @(
        "n7.1.4-39-ga5faeca88f-20260611",
        "c8bc13fb4079fc477ecca83985d913d43d1a69efdb974ebc1f37490e603b5b79",
        "--enable-shared",
        "--disable-static",
        "--enable-gpl",
        "--enable-nonfree",
        "KIVO_FFMPEG_ROOT"
    )
    foreach ($evidence in $requiredEvidence) {
        if ($manifest -notmatch [regex]::Escape($evidence)) {
            $violations += "FFMPEG_PROVENANCE: missing evidence '$evidence'"
        }
    }
}

$fixtureManifestPath =
    Join-Path $ProjectRoot "docs\dependencies\generated_audio_test_fixtures.md"
if (-not (Test-Path $fixtureManifestPath)) {
    $violations += "TEST_FIXTURE_PROVENANCE: generated fixture manifest is missing"
} else {
    $fixtureManifest = Get-Content $fixtureManifestPath -Raw
    foreach ($fixtureHash in @(
        "101b74ef127db194060ca1240e16551f92d3449d75f0248c3894963290f53de1",
        "0239214dc0f0a21376185d71a482f4f0b1b704dc315a5432d2acb2d23783ce46",
        "a111e9605c587b55f99c43998d86220ea234c15c12b6b31ed29b015a8a744a42"
    )) {
        if ($fixtureManifest -notmatch [regex]::Escape($fixtureHash)) {
            $violations += "TEST_FIXTURE_PROVENANCE: missing hash '$fixtureHash'"
        }
    }
}

Write-Host "--- Checks ---"
Write-Host "1. No vendored dependency directories"
Write-Host "2. No configure/build/test dependency acquisition"
Write-Host "3. No package-manager manifests or git submodules"
Write-Host "4. External KIVO_FFMPEG_ROOT integration"
Write-Host "5. Locked FFmpeg LGPL shared provenance"
Write-Host "6. Generated test-media provenance and hashes"
Write-Host ""

if ($violations.Count -eq 0) {
    Write-Host "PASS"
    Write-Host "External dependency policy and FFmpeg provenance confirmed."
    exit 0
}

Write-Host "FAIL"
Write-Host "Violations found ($($violations.Count)):"
foreach ($violation in $violations) {
    Write-Host "  - $violation"
}
exit 1
