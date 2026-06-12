# =============================================================================
# check_no_audio_runtime_gate.ps1
# Authorized Audio Runtime Gate
# =============================================================================

param(
    [string]$ProjectRoot
)

$ErrorActionPreference = "Stop"

if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
}
$ProjectRoot = (Resolve-Path $ProjectRoot).Path

Write-Host "=== Authorized Audio Runtime Gate ==="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host ""

$violations = @()

$allowedContractPaths = @(
    "include\kivo\core\contract",
    "include\kivo\core\render",
    "include\kivo\core\output",
    "include\kivo\core\processing",
    "include\kivo\core\decode",
    "include\kivo\core\playback",
    "include\kivo\adapters\ffmpeg",
    "include\kivo\source\local",
    "include\kivo\testing",
    "include\kivo\platform\windows\wasapi",
    "include\kivo\host\abi",
    "src\core\contract",
    "src\core\render\queue",
    "src\core\render\pump",
    "src\core\output",
    "src\core\processing",
    "src\core\playback\pipeline",
    "src\core\playback\gapless",
    "src\core\playback\recovery",
    "src\core\playback\runtime",
    "src\core\playback\session",
    "src\testing",
    "src\source\local",
    "src\adapters\ffmpeg",
    "src\platform\windows\wasapi",
    "src\host\abi",
    "tests\contracts",
    "tests\render_boundary",
    "tests\output_truth",
    "tests\audio_processing",
    "tests\render_queue",
    "tests\decode_boundary",
    "tests\ffmpeg_decode",
    "tests\playback_session",
    "tests\playback_pipeline",
    "tests\playback_gapless",
    "tests\playback_runtime",
    "tests\fake_renderer",
    "tests\stability",
    "tests\platform_windows\wasapi",
    "tests\hardware\device_matrix",
    "tests\hardware\wasapi",
    "tests\hardware\decode_output",
    "tests\hardware\host_abi",
    "tests\host_abi"
)

$excludedPathPatterns = @("*\docs\*", "*\tools\*", "*\.build\*", "*\build\*", "*\.git\*")

function Get-RelativePath {
    param([string]$FullPath, [string]$Root)
    $normalizedFull = $FullPath.ToLower()
    $normalizedRoot = $Root.ToLower()
    if ($normalizedFull.StartsWith($normalizedRoot)) {
        $relative = $FullPath.Substring($Root.Length)
        return $relative.TrimStart("\", "/")
    }
    return $FullPath
}

function Test-IsAllowedContractPath {
    param([string]$RelativePath)
    $normalizedRel = $RelativePath.ToLower().Replace("/", "\")
    foreach ($allowed in $allowedContractPaths) {
        $normalizedAllowed = $allowed.ToLower()
        if ($normalizedRel -eq $normalizedAllowed -or $normalizedRel.StartsWith("$normalizedAllowed\")) {
            return $true
        }
        if ($normalizedAllowed.StartsWith("$normalizedRel\")) {
            return $true
        }
    }
    return $false
}

function Test-IsExcludedPath {
    param([string]$FullPath)
    foreach ($pattern in $excludedPathPatterns) {
        if ($FullPath -like $pattern) {
            return $true
        }
    }
    return $false
}

$runtimeDirs = @("playback", "platform", "decode", "dsp", "examples", "demo", "sample_player")
foreach ($dir in $runtimeDirs) {
    $dirPath = Join-Path $ProjectRoot $dir
    if (Test-Path $dirPath) {
        $violations += "RUNTIME_CARRIER_FOUND: $dir/ directory exists"
    }
}

foreach ($dir in @("src", "tests")) {
    $dirPath = Join-Path $ProjectRoot $dir
    if (Test-Path $dirPath) {
        $nonContractItems = Get-ChildItem -Path $dirPath -Recurse -Force -ErrorAction SilentlyContinue |
            Where-Object {
                -not (Test-IsExcludedPath $_.FullName) -and
                -not (Test-IsAllowedContractPath (Get-RelativePath -FullPath $_.FullName -Root $ProjectRoot))
            }
        if ($nonContractItems) {
            foreach ($item in $nonContractItems) {
                $relPath = Get-RelativePath -FullPath $item.FullName -Root $ProjectRoot
                $violations += "RUNTIME_CARRIER_FOUND: Non-contract item in $dir/: $relPath"
            }
        }
    }
}

$sourceFiles = Get-ChildItem -Path $ProjectRoot -Recurse -File -Include "*.cpp","*.h","*.hpp","*.c" -ErrorAction SilentlyContinue |
    Where-Object { -not (Test-IsExcludedPath $_.FullName) }
if ($sourceFiles) {
    foreach ($sf in $sourceFiles) {
        $relativePath = Get-RelativePath -FullPath $sf.FullName -Root $ProjectRoot
        if (-not (Test-IsAllowedContractPath $relativePath)) {
            $violations += "RUNTIME_CARRIER_FOUND: Source file exists: $relativePath"
        }
    }
}

$cmakePath = Join-Path $ProjectRoot "CMakeLists.txt"
if (Test-Path $cmakePath) {
    $cmakeContent = Get-Content $cmakePath -Raw
    $lines = $cmakeContent -split "`n"
    $allowedTargetNames = @(
        "kivo_core_contract",
        "kivo_contract_tests",
        "contract_tests",
        "kivo_capability_tests",
        "kivo_core_render",
        "kivo_output_truth",
        "kivo_output_truth_tests",
        "output_truth_tests",
        "kivo_audio_processing",
        "kivo_audio_processing_tests",
        "audio_processing_tests",
        "kivo_render_queue",
        "kivo_decode_boundary",
        "kivo_decode_boundary_tests",
        "decode_boundary_tests",
        "kivo_ffmpeg_avcodec",
        "kivo_ffmpeg_avformat",
        "kivo_ffmpeg_avutil",
        "kivo_ffmpeg_swresample",
        "kivo_ffmpeg_decode_adapter",
        "kivo_ffmpeg_test_fixtures",
        "kivo_ffmpeg_decode_tests",
        "ffmpeg_decode_tests",
        "kivo_ffmpeg_wasapi_playback_smoke",
        "kivo_playback_session",
        "kivo_playback_session_tests",
        "playback_session_tests",
        "kivo_playback_pipeline",
        "kivo_playback_pipeline_tests",
        "playback_pipeline_tests",
        "kivo_playback_gapless",
        "kivo_playback_gapless_tests",
        "playback_gapless_tests",
        "kivo_playback_runtime",
        "kivo_playback_runtime_tests",
        "playback_runtime_tests",
        "kivo_render_queue_tests",
        "render_queue_tests",
        "kivo_render_boundary_tests",
        "render_boundary_tests",
        "kivo_fake_render_backend",
        "kivo_fake_renderer_tests",
        "fake_renderer_tests",
        "kivo_wasapi_renderer",
        "kivo_wasapi_tests",
        "wasapi_tests",
        "kivo_wasapi_tone_smoke",
        "kivo_wasapi_device_matrix",
        "kivo_stability_tests",
        "stability_tests",
        "kivo_public_header_checks",
        "kivo_public_c_header_checks",
        "kivo_host_abi",
        "kivo_host_abi_tests",
        "kivo_host_abi_playback_smoke",
        "host_abi_tests"
    )
    foreach ($pattern in @("add_executable", "add_library")) {
        foreach ($line in $lines) {
            if ($line -match $pattern -and $line -notmatch "^\s*#") {
                $isAllowed = $false
                if ($line -match "add_library\(\$\{target_name\}\s+SHARED\s+IMPORTED\s+GLOBAL\)") {
                    $isAllowed = $true
                }
                foreach ($allowed in $allowedTargetNames) {
                    if ($line -match [regex]::Escape($allowed)) {
                        $isAllowed = $true
                        break
                    }
                }
                if (-not $isAllowed) {
                    $violations += "RUNTIME_CARRIER_FOUND: CMakeLists.txt contains unauthorized target: $($line.Trim())"
                }
            }
        }
    }

    foreach ($line in $lines) {
        if ($line -match "target_sources" -and $line -notmatch "^\s*#") {
            $violations += "RUNTIME_CARRIER_FOUND: CMakeLists.txt contains uncommented: target_sources"
        }
    }
}

$vendorDirs = @("vendor", "third_party", "bin", "lib")
foreach ($dir in $vendorDirs) {
    $dirPath = Join-Path $ProjectRoot $dir
    if (Test-Path $dirPath) {
        $violations += "RUNTIME_CARRIER_FOUND: $dir/ directory exists"
    }
}

Write-Host "--- Scan Scope ---"
Write-Host "Checking: runtime directories, source files, CMake targets, vendor dirs"
Write-Host "Excluding: docs/, tools/, .build/, build/, .git/"
Write-Host "Allowed paths: core boundaries, deterministic backend, quarantined FFmpeg/WASAPI adapters, and tests"
Write-Host "Allowed CMake targets: contract, render, FFmpeg, WASAPI, and validation targets"
Write-Host ""

if ($violations.Count -eq 0) {
    Write-Host "PASS"
    Write-Host "NO_UNAUTHORIZED_RUNTIME_CARRIER_FOUND"
    Write-Host "Only roadmap-authorized audio runtime implementation was detected."
    exit 0
}

Write-Host "FAIL"
Write-Host "RUNTIME_CARRIER_FOUND_FAIL"
Write-Host "Violations found ($($violations.Count)):"
foreach ($v in $violations) {
    Write-Host "  - $v"
}
exit 1
