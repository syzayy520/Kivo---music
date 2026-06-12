# =============================================================================
# check_forbidden_token_gate.ps1
# Forbidden Token Gate — Layered Scanning
# =============================================================================
# Scans for platform implementation tokens with layered scope:
#   A. Strict Implementation Scope (src/, core/, platform/, etc.) — zero tolerance
#   B. Root Skeleton Scope (README, CMake) — no implementation patterns
#   C. Docs/Policy Scope — DOC_MENTION_ALLOWED
#   D. Gate Script Scope — GATE_LIST_ALLOWED
#   E. Generated Output Scope — ignored
# =============================================================================

param(
    [string]$ProjectRoot
)

$ErrorActionPreference = "Stop"

# Resolve PROJECT_ROOT — always resolve to absolute path
if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
}
$ProjectRoot = (Resolve-Path $ProjectRoot).Path

Write-Host "=== Forbidden Token Gate ==="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host ""

# --- Strict Implementation Scope ---
$strictDirs = @("include", "src", "core", "platform", "decode", "playback", "tests", "examples", "demo", "sample_player")
$windowsPlatformTokens = @(
    "#include <Windows.h>",
    "#include <windows.h>",
    "#include <audioclient.h>",
    "#include <mmdeviceapi.h>",
    "#include <endpointvolume.h>",
    "IAudioClient",
    "IAudioRenderClient",
    "GetBuffer",
    "ReleaseBuffer"
)
$ffmpegImplementationTokens = @(
    "#include <avcodec.h>",
    "#include <avformat.h>",
    "#include <avutil.h>",
    "#include <swresample.h>",
    "#include <libavcodec/",
    "#include <libavformat/",
    "#include <libavutil/",
    "#include <libswresample/",
    "AVCodecContext",
    "AVFormatContext",
    "AVIOContext",
    "AVFrame",
    "AVPacket",
    "SwrContext"
)
$implementationTokens = @(
    "#include <Windows.h>",
    "#include <windows.h>",
    "#include <audioclient.h>",
    "#include <mmdeviceapi.h>",
    "#include <endpointvolume.h>",
    "#include <avcodec.h>",
    "#include <avformat.h>",
    "#include <avutil.h>",
    "#include <swresample.h>",
    "#include <libavcodec/",
    "#include <libavformat/",
    "#include <libavutil/",
    "#include <libswresample/",
    "IAudioClient",
    "IAudioRenderClient",
    "AVCodecContext",
    "AVFormatContext",
    "AVIOContext",
    "AVFrame",
    "AVPacket",
    "SwrContext",
    "mpv_handle",
    "GetBuffer",
    "ReleaseBuffer"
)

$violations = @()

# Check strict implementation scope
foreach ($dir in $strictDirs) {
    $dirPath = Join-Path $ProjectRoot $dir
    if (Test-Path $dirPath) {
        $files = Get-ChildItem -Path $dirPath -Recurse -File -Include "*.cpp","*.h","*.hpp","*.c" -ErrorAction SilentlyContinue
        foreach ($file in $files) {
            $content = Get-Content $file.FullName -Raw -ErrorAction SilentlyContinue
            if ($content) {
                $relativePath = $file.FullName.Substring($ProjectRoot.Length).TrimStart("\", "/").Replace("/", "\")
                $authorizedWindowsPath =
                    $relativePath.StartsWith("src\platform\windows\wasapi\", [System.StringComparison]::OrdinalIgnoreCase) -or
                    $relativePath.StartsWith("tests\platform_windows\wasapi\", [System.StringComparison]::OrdinalIgnoreCase) -or
                    $relativePath.StartsWith("tests\hardware\device_matrix\platform\windows\", [System.StringComparison]::OrdinalIgnoreCase) -or
                    $relativePath.StartsWith("tests\hardware\wasapi\", [System.StringComparison]::OrdinalIgnoreCase) -or
                    $relativePath.StartsWith("tests\stability\platform\windows\", [System.StringComparison]::OrdinalIgnoreCase)
                $authorizedFfmpegPath =
                    $relativePath.StartsWith("src\adapters\ffmpeg\", [System.StringComparison]::OrdinalIgnoreCase)
                foreach ($token in $implementationTokens) {
                    if ($authorizedWindowsPath -and $token -in $windowsPlatformTokens) {
                        continue
                    }
                    if ($authorizedFfmpegPath -and $token -in $ffmpegImplementationTokens) {
                        continue
                    }
                    if ($content -match [regex]::Escape($token)) {
                        $violations += "STRICT_VIOLATION: ${relativePath}: $token"
                    }
                }
            }
        }
    }
}

# --- Deterministic Fake Backend Scope ---
# The fake backend must remain bounded and timing-independent.
$fakeBackendDirs = @("include\kivo\testing", "src\testing")
$fakeBackendTokens = @(
    "Sleep(",
    "sleep_for",
    "sleep_until",
    "std::vector",
    "std::deque",
    "std::list",
    "malloc(",
    "calloc(",
    "realloc(",
    "operator new"
)

foreach ($dir in $fakeBackendDirs) {
    $dirPath = Join-Path $ProjectRoot $dir
    if (Test-Path $dirPath) {
        $files = Get-ChildItem -Path $dirPath -Recurse -File -Include "*.cpp","*.h","*.hpp" -ErrorAction SilentlyContinue
        foreach ($file in $files) {
            $content = Get-Content $file.FullName -Raw -ErrorAction SilentlyContinue
            if ($content) {
                foreach ($token in $fakeBackendTokens) {
                    if ($content -match [regex]::Escape($token)) {
                        $violations += "FAKE_DETERMINISM_VIOLATION: $($file.Name): $token"
                    }
                }
            }
        }
    }
}

# --- Root Skeleton Scope ---
# P0-C: CMakeLists.txt may contain contract-only targets
$rootFiles = @("README.md", "CMakePresets.json")
$rootBannedPatterns = @(
    "find_package(FFmpeg",
    "find_package(ffmpeg",
    "FetchContent",
    "main.cpp",
    "include <Windows.h>",
    "include <audioclient.h>"
)

foreach ($file in $rootFiles) {
    $filePath = Join-Path $ProjectRoot $file
    if (Test-Path $filePath) {
        $content = Get-Content $filePath -Raw
        foreach ($pattern in $rootBannedPatterns) {
            if ($content -match [regex]::Escape($pattern)) {
                $violations += "ROOT_SKELETON_VIOLATION: $file contains: $pattern"
            }
        }
    }
}

# --- CMakeLists.txt P0-C Scope Check ---
# Only authorized P0-C targets/patterns are allowed
$cmakePath = Join-Path $ProjectRoot "CMakeLists.txt"
if (Test-Path $cmakePath) {
    $cmakeContent = Get-Content $cmakePath -Raw
    $cmakeBannedPatterns = @(
        "find_package(FFmpeg",
        "find_package(ffmpeg",
        "FetchContent",
        "include <Windows.h>",
        "include <audioclient.h>"
    )
    foreach ($pattern in $cmakeBannedPatterns) {
        if ($cmakeContent -match [regex]::Escape($pattern)) {
            $violations += "ROOT_SKELETON_VIOLATION: CMakeLists.txt contains: $pattern"
        }
    }
    # Check for non-contract targets
    $cmakeLines = $cmakeContent -split "`n"
    $allowedTargets = @(
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
        "kivo_hires_pcm_tests",
        "hires_pcm_tests",
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
    foreach ($line in $cmakeLines) {
        if ($line -match "^\s*add_executable\((\w+)" -or $line -match "^\s*add_library\((\w+)") {
            $targetName = $Matches[1]
            if ($targetName -notin $allowedTargets) {
                $violations += "ROOT_SKELETON_VIOLATION: CMakeLists.txt contains unauthorized target: $targetName"
            }
        }
    }
}

# --- Report ---
Write-Host "--- Scan Scope ---"
Write-Host "Strict Implementation: src/, core/, platform/, decode/, playback/, tests/, examples/, demo/, sample_player/"
Write-Host "Root Skeleton: README.md, CMakeLists.txt, CMakePresets.json"
Write-Host "Docs/Policy: excluded (DOC_MENTION_ALLOWED)"
Write-Host "Gate Scripts: excluded (GATE_LIST_ALLOWED)"
Write-Host "Generated Output: .build/, build/, out/ (GENERATED_BUILD_DIR_IGNORED)"
Write-Host ""

if ($violations.Count -eq 0) {
    Write-Host "PASS"
    Write-Host "No forbidden token violations found in implementation or skeleton scope."
    exit 0
} else {
    Write-Host "FAIL"
    Write-Host "Violations found ($($violations.Count)):"
    foreach ($v in $violations) {
        Write-Host "  - $v"
    }
    exit 1
}
