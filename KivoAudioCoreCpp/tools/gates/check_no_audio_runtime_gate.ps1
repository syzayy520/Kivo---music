# =============================================================================
# check_no_audio_runtime_gate.ps1
# No Audio Runtime Gate — Runtime Carrier Detection
# =============================================================================

param(
    [string]$ProjectRoot
)

$ErrorActionPreference = "Stop"

if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
}
$ProjectRoot = (Resolve-Path $ProjectRoot).Path

Write-Host "=== No Audio Runtime Gate ==="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host ""

$violations = @()

$allowedContractPaths = @(
    "include\kivo\core\contract",
    "include\kivo\core\render",
    "include\kivo\testing",
    "src\core\contract",
    "src\testing",
    "tests\contracts",
    "tests\render_boundary",
    "tests\fake_renderer"
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
        "kivo_render_boundary_tests",
        "render_boundary_tests",
        "kivo_fake_render_backend",
        "kivo_fake_renderer_tests",
        "fake_renderer_tests",
        "kivo_public_header_checks"
    )
    foreach ($pattern in @("add_executable", "add_library")) {
        foreach ($line in $lines) {
            if ($line -match $pattern -and $line -notmatch "^\s*#") {
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
Write-Host "Allowed paths: core boundaries, deterministic testing backend, and their tests"
Write-Host "Allowed CMake targets: contract, render-boundary, fake-renderer, and validation targets"
Write-Host ""

if ($violations.Count -eq 0) {
    Write-Host "PASS"
    Write-Host "NO_RUNTIME_CARRIER_FOUND"
    Write-Host "No audio runtime implementation detected."
    exit 0
}

Write-Host "FAIL"
Write-Host "RUNTIME_CARRIER_FOUND_FAIL"
Write-Host "Violations found ($($violations.Count)):"
foreach ($v in $violations) {
    Write-Host "  - $v"
}
exit 1
