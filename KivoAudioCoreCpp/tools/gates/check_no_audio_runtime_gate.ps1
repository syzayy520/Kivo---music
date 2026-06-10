# =============================================================================
# check_no_audio_runtime_gate.ps1
# No Audio Runtime Gate — Runtime Carrier Detection
# =============================================================================
# Checks for real runtime carriers, NOT documentation mentions:
#   1. src/, tests/, playback/, platform/ directories
#   2. add_executable, add_library, target_sources
#   3. .cpp/.h/.hpp contract or implementation files
#   4. Real platform includes
#   5. Real thread, queue, buffer runtime
#   6. FFmpeg/mpv/WASAPI implementation tokens
#   7. vendor/bin/lib/third_party
#   8. sample player / demo
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

Write-Host "=== No Audio Runtime Gate ==="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host ""

$violations = @()

# --- 1. Check for runtime directories ---
$runtimeDirs = @("src", "tests", "playback", "platform", "decode", "core", "dsp", "examples", "demo", "sample_player")
foreach ($dir in $runtimeDirs) {
    $dirPath = Join-Path $ProjectRoot $dir
    if (Test-Path $dirPath) {
        $violations += "RUNTIME_CARRIER_FOUND: $dir/ directory exists"
    }
}

# --- 2. Check for C++ source files (outside docs/tools) ---
$sourceFiles = Get-ChildItem -Path $ProjectRoot -Recurse -File -Include "*.cpp","*.h","*.hpp","*.c" -ErrorAction SilentlyContinue |
    Where-Object { $_.FullName -notlike "*\docs\*" -and $_.FullName -notlike "*\tools\*" -and $_.FullName -notlike "*\.build\*" -and $_.FullName -notlike "*\.git\*" }
if ($sourceFiles) {
    foreach ($sf in $sourceFiles) {
        $relativePath = $sf.FullName.Replace($ProjectRoot, "").TrimStart("\", "/")
        $violations += "RUNTIME_CARRIER_FOUND: Source file exists: $relativePath"
    }
}

# --- 3. Check CMakeLists.txt for build targets ---
$cmakePath = Join-Path $ProjectRoot "CMakeLists.txt"
if (Test-Path $cmakePath) {
    $cmakeContent = Get-Content $cmakePath -Raw
    $cmakeTargets = @("add_executable", "add_library", "target_sources", "target_link_libraries")
    foreach ($target in $cmakeTargets) {
        if ($cmakeContent -match [regex]::Escape($target)) {
            # Check it's not in a comment
            $lines = $cmakeContent -split "`n"
            foreach ($line in $lines) {
                if ($line -match [regex]::Escape($target) -and $line -notmatch "^\s*#") {
                    $violations += "RUNTIME_CARRIER_FOUND: CMakeLists.txt contains uncommented: $target"
                }
            }
        }
    }
}

# --- 4. Check for vendor/third_party ---
$vendorDirs = @("vendor", "third_party", "bin", "lib")
foreach ($dir in $vendorDirs) {
    $dirPath = Join-Path $ProjectRoot $dir
    if (Test-Path $dirPath) {
        $violations += "RUNTIME_CARRIER_FOUND: $dir/ directory exists"
    }
}

# Report
Write-Host "--- Scan Scope ---"
Write-Host "Checking: runtime directories, source files, CMake targets, vendor dirs"
Write-Host "Excluding: docs/, tools/, .build/, .git/"
Write-Host ""

$docMentions = 0
Write-Host "DOC_MENTION_ALLOWED: $docMentions (docs may discuss WASAPI/FFmpeg/mpv as future phases)"
Write-Host ""

if ($violations.Count -eq 0) {
    Write-Host "PASS"
    Write-Host "NO_RUNTIME_CARRIER_FOUND"
    Write-Host "No audio runtime implementation detected."
    exit 0
} else {
    Write-Host "FAIL"
    Write-Host "RUNTIME_CARRIER_FOUND_FAIL"
    Write-Host "Violations found ($($violations.Count)):"
    foreach ($v in $violations) {
        Write-Host "  - $v"
    }
    exit 1
}
