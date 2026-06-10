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

# Resolve PROJECT_ROOT
if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
    $ProjectRoot = Resolve-Path $ProjectRoot
}

Write-Host "=== Forbidden Token Gate ==="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host ""

# --- Strict Implementation Scope ---
$strictDirs = @("src", "core", "platform", "decode", "playback", "tests", "examples", "demo", "sample_player")
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
    "IAudioClient",
    "IAudioRenderClient",
    "AVFrame",
    "AVPacket",
    "mpv_handle",
    "GetBuffer",
    "ReleaseBuffer"
)

$violations = @()

# Check strict implementation scope
foreach ($dir in $strictDirs) {
    $dirPath = Join-Path $ProjectRoot $dir
    if (Test-Path $dirPath) {
        foreach ($token in $implementationTokens) {
            $results = Get-ChildItem -Path $dirPath -Recurse -File -Include "*.cpp","*.h","*.hpp","*.c" -ErrorAction SilentlyContinue |
                Select-String -Pattern [regex]::Escape($token) -SimpleMatch
            foreach ($r in $results) {
                $violations += "STRICT_VIOLATION: $($r.Path):$($r.LineNumber): $token"
            }
        }
    }
}

# --- Root Skeleton Scope ---
$rootFiles = @("README.md", "CMakeLists.txt", "CMakePresets.json")
$rootBannedPatterns = @(
    "add_executable",
    "add_library",
    "target_sources",
    "target_link_libraries",
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
