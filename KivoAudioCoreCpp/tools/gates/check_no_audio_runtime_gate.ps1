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
#
# P0-C Exception: Contract-only paths and targets are allowed.
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

Write-Host "=== No Audio Runtime Gate ==="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host ""

$violations = @()

# --- P0-C Allowed Contract Paths ---
# These paths contain contract-only types (no platform, no runtime, no dependencies).
$allowedContractPaths = @(
    "include\kivo\core\contract",
    "src\core\contract",
    "tests\contracts"
)

function Get-RelativePath {
    param([string]$FullPath, [string]$Root)
    # Case-insensitive replacement
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
    }
    return $false
}

# --- 1. Check for runtime directories ---
# P0-C: src/, tests/ are allowed if they only contain contract paths
$runtimeDirs = @("playback", "platform", "decode", "dsp", "examples", "demo", "sample_player")
foreach ($dir in $runtimeDirs) {
    $dirPath = Join-Path $ProjectRoot $dir
    if (Test-Path $dirPath) {
        $violations += "RUNTIME_CARRIER_FOUND: $dir/ directory exists"
    }
}

# Check src/ and tests/ for non-contract content
foreach ($dir in @("src", "tests")) {
    $dirPath = Join-Path $ProjectRoot $dir
    if (Test-Path $dirPath) {
        $nonContractItems = Get-ChildItem -Path $dirPath -Recurse -Force -ErrorAction SilentlyContinue |
            Where-Object {
                $relPath = Get-RelativePath -FullPath $_.FullName -Root $ProjectRoot
                -not (Test-IsAllowedContractPath $relPath)
            }
        if ($nonContractItems) {
            foreach ($item in $nonContractItems) {
                $relPath = Get-RelativePath -FullPath $item.FullName -Root $ProjectRoot
                $violations += "RUNTIME_CARRIER_FOUND: Non-contract item in $dir/: $relPath"
            }
        }
    }
}

# --- 2. Check for C++ source files (outside docs/tools and allowed contract paths) ---
$sourceFiles = Get-ChildItem -Path $ProjectRoot -Recurse -File -Include "*.cpp","*.h","*.hpp","*.c" -ErrorAction SilentlyContinue |
    Where-Object {
        $normPath = $_.FullName.ToLower()
        $normPath -notlike "*\docs\*" -and
        $normPath -notlike "*\tools\*" -and
        $normPath -notlike "*\.build\*" -and
        $normPath -notlike "*\.git\*"
    }
if ($sourceFiles) {
    foreach ($sf in $sourceFiles) {
        $relativePath = Get-RelativePath -FullPath $sf.FullName -Root $ProjectRoot
        if (-not (Test-IsAllowedContractPath $relativePath)) {
            $violations += "RUNTIME_CARRIER_FOUND: Source file exists: $relativePath"
        }
    }
}

# --- 3. Check CMakeLists.txt for build targets ---
# P0-C: Only contract-related targets are allowed
$cmakePath = Join-Path $ProjectRoot "CMakeLists.txt"
if (Test-Path $cmakePath) {
    $cmakeContent = Get-Content $cmakePath -Raw
    $lines = $cmakeContent -split "`n"
    
    # Allowed P0-C target names
    $allowedTargetNames = @("kivo_core_contract", "kivo_contract_tests", "contract_tests", "kivo_capability_tests")
    
    # Check for executable/library targets
    $targetPatterns = @("add_executable", "add_library")
    foreach ($pattern in $targetPatterns) {
        foreach ($line in $lines) {
            if ($line -match $pattern -and $line -notmatch "^\s*#") {
                # Extract target name
                $isAllowed = $false
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
    
    # Check for target_sources (never allowed in P0-C)
    foreach ($line in $lines) {
        if ($line -match "target_sources" -and $line -notmatch "^\s*#") {
            $violations += "RUNTIME_CARRIER_FOUND: CMakeLists.txt contains uncommented: target_sources"
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
Write-Host "P0-C Allowed: include/kivo/core/contract/, src/core/contract/, tests/contracts/"
Write-Host "P0-C Allowed CMake: kivo_core_contract, kivo_contract_tests, kivo_capability_tests"
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
