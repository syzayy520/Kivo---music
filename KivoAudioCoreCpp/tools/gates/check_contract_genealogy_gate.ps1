# =============================================================================
# check_contract_genealogy_gate.ps1
# Contract Genealogy Gate — Flatness and Family Tree Enforcement
# =============================================================================
# Validates that include/kivo/core/contract/ and tests/contracts/ follow
# the genealogy tree structure. Prevents flatness regression.
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

Write-Host "=== Contract Genealogy Gate ==="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host ""

$violations = @()

# =============================================================================
# Rule G1: Production root allows only foundation primitives + family dirs
# =============================================================================
$contractRoot = Join-Path $ProjectRoot "include\kivo\core\contract"

$allowedProductionRoot = @(
    "result.hpp",
    "generation_id.hpp",
    "sample_position.hpp"
)

$allowedProductionDirs = @(
    "format",
    "clock",
    "seek"
)

if (Test-Path $contractRoot) {
    $items = Get-ChildItem -Path $contractRoot
    foreach ($item in $items) {
        if ($item.PSIsContainer) {
            # Directory
            if ($item.Name -notin $allowedProductionDirs) {
                $violations += "G1: Unauthorized production directory: include/kivo/core/contract/$($item.Name)/"
            }
        } else {
            # File
            if ($item.Name -notin $allowedProductionRoot) {
                $violations += "G2: Unauthorized production file at root: include/kivo/core/contract/$($item.Name)"
            }
        }
    }
} else {
    $violations += "G1: Missing directory: include/kivo/core/contract/"
}

# =============================================================================
# Rule G3: Each production family subdir contains exactly 1 .hpp file
# =============================================================================
foreach ($dir in $allowedProductionDirs) {
    $dirPath = Join-Path $contractRoot $dir
    if (Test-Path $dirPath) {
        $hppFiles = Get-ChildItem -Path $dirPath -Filter "*.hpp"
        if ($hppFiles.Count -ne 1) {
            $violations += "G3: Production family '$dir' should contain exactly 1 .hpp file, found $($hppFiles.Count)"
        }
    } else {
        $violations += "G3: Missing production family directory: include/kivo/core/contract/$dir/"
    }
}

# =============================================================================
# Rule G4: Test root allows only runner files + family dirs
# =============================================================================
$testsRoot = Join-Path $ProjectRoot "tests\contracts"

$allowedTestRootFiles = @(
    "contract_tests_main.cpp",
    "contract_tests_main.h"
)

$allowedTestDirs = @(
    "foundation",
    "format",
    "clock",
    "seek"
)

if (Test-Path $testsRoot) {
    $items = Get-ChildItem -Path $testsRoot
    foreach ($item in $items) {
        if ($item.PSIsContainer) {
            # Directory
            if ($item.Name -notin $allowedTestDirs) {
                $violations += "G4: Unauthorized test directory: tests/contracts/$($item.Name)/"
            }
        } else {
            # File
            if ($item.Name -notin $allowedTestRootFiles) {
                $violations += "G5: Unauthorized test file at root: tests/contracts/$($item.Name)"
            }
        }
    }
} else {
    $violations += "G4: Missing directory: tests/contracts/"
}

# =============================================================================
# Rule G6: Each test family subdir contains exactly the expected test file(s)
# =============================================================================
$expectedTestFiles = @{
    "foundation" = @("result_tests.cpp", "generation_id_tests.cpp", "sample_position_tests.cpp")
    "format"     = @("audio_format_descriptor_tests.cpp")
    "clock"      = @("clock_domain_tests.cpp")
    "seek"       = @("seek_flush_tests.cpp")
}

foreach ($dir in $allowedTestDirs) {
    $dirPath = Join-Path $testsRoot $dir
    if (Test-Path $dirPath) {
        $actualFiles = Get-ChildItem -Path $dirPath -Filter "*.cpp" | ForEach-Object { $_.Name }
        $expectedFiles = $expectedTestFiles[$dir]

        foreach ($f in $actualFiles) {
            if ($f -notin $expectedFiles) {
                $violations += "G6: Unexpected test file: tests/contracts/$dir/$f"
            }
        }
        foreach ($f in $expectedFiles) {
            if ($f -notin $actualFiles) {
                $violations += "G6: Missing test file: tests/contracts/$dir/$f"
            }
        }
    } else {
        $violations += "G6: Missing test family directory: tests/contracts/$dir/"
    }
}

# =============================================================================
# Rule G7: No bucket directories
# =============================================================================
$bucketDirs = @("helper", "utils", "common", "misc", "glue", "facade", "types", "defs")

foreach ($bucket in $bucketDirs) {
    $inContract = Join-Path $contractRoot $bucket
    $inTests = Join-Path $testsRoot $bucket
    if (Test-Path $inContract) {
        $violations += "G7: Bucket directory in production: include/kivo/core/contract/$bucket/"
    }
    if (Test-Path $inTests) {
        $violations += "G7: Bucket directory in tests: tests/contracts/$bucket/"
    }
}

# =============================================================================
# Rule G8: Each production family has a corresponding test family
# =============================================================================
foreach ($dir in $allowedProductionDirs) {
    $testDir = Join-Path $testsRoot $dir
    if (-not (Test-Path $testDir)) {
        $violations += "G8: Production family '$dir' has no corresponding test family: tests/contracts/$dir/"
    }
}

# =============================================================================
# Rule G9: Foundation test family exists
# =============================================================================
$foundationTestDir = Join-Path $testsRoot "foundation"
if (-not (Test-Path $foundationTestDir)) {
    $violations += "G9: Missing foundation test family: tests/contracts/foundation/"
}

# =============================================================================
# Report
# =============================================================================
if ($violations.Count -eq 0) {
    Write-Host "PASS"
    Write-Host "Contract genealogy tree is valid."
    exit 0
} else {
    Write-Host "FAIL"
    Write-Host "Violations found ($($violations.Count)):"
    foreach ($v in $violations) {
        Write-Host "  - $v"
    }
    exit 1
}
