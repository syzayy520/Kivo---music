# =============================================================================
# check_contract_genealogy_gate.ps1
# Contract Genealogy Gate — Family Tree Enforcement
# =============================================================================

param(
    [string]$ProjectRoot
)

$ErrorActionPreference = "Stop"

if (-not $ProjectRoot) {
    $ProjectRoot = Join-Path $PSScriptRoot "..\.."
}
$ProjectRoot = (Resolve-Path $ProjectRoot).Path

Write-Host "=== Contract Genealogy Gate ==="
Write-Host "PROJECT_ROOT: $ProjectRoot"
Write-Host ""

$violations = @()

$contractRoot = Join-Path $ProjectRoot "include\kivo\core\contract"
$testsRoot = Join-Path $ProjectRoot "tests\contracts"

$allowedProductionRootFiles = @(
    "result.hpp",
    "generation_id.hpp",
    "sample_position.hpp"
)

$allowedProductionDirs = @(
    "format",
    "clock",
    "seek",
    "capability",
    "output",
    "realtime",
    "generation",
    "buffer",
    "queue",
    "source",
    "cue",
    "error",
    "state",
    "command",
    "observability",
    "policy"
)

$allowedTestRootFiles = @(
    "contract_tests_main.cpp",
    "contract_tests_main.h"
)

$allowedTestDirs = @(
    "foundation",
    "composability",
    "format",
    "clock",
    "seek",
    "capability",
    "output",
    "realtime",
    "generation",
    "buffer",
    "queue",
    "source",
    "cue",
    "error",
    "state",
    "command",
    "observability",
    "policy"
)

$blockedBucketDirs = @("helper", "utils", "common", "misc", "glue", "facade", "types", "defs")

function Get-RelativePath {
    param([string]$FullPath, [string]$Root)
    return $FullPath.Substring($Root.Length).TrimStart("\", "/").Replace("/", "\")
}

function Test-BucketDirectoryNames {
    param([string]$RootPath, [string]$Label)

    if (-not (Test-Path $RootPath)) {
        return
    }

    $dirs = Get-ChildItem -Path $RootPath -Directory -Recurse -Force -ErrorAction SilentlyContinue
    foreach ($dir in $dirs) {
        if ($dir.Name -in $blockedBucketDirs) {
            $relative = Get-RelativePath -FullPath $dir.FullName -Root $ProjectRoot
            $script:violations += "$Label bucket directory: $relative/"
        }
    }
}

if (Test-Path $contractRoot) {
    $items = Get-ChildItem -Path $contractRoot
    foreach ($item in $items) {
        if ($item.PSIsContainer) {
            if ($item.Name -notin $allowedProductionDirs) {
                $violations += "G1: Unauthorized production directory: include/kivo/core/contract/$($item.Name)/"
            }
        } else {
            if ($item.Name -notin $allowedProductionRootFiles) {
                $violations += "G2: Unauthorized production file at root: include/kivo/core/contract/$($item.Name)"
            }
        }
    }
} else {
    $violations += "G1: Missing directory: include/kivo/core/contract/"
}

if (Test-Path $testsRoot) {
    $items = Get-ChildItem -Path $testsRoot
    foreach ($item in $items) {
        if ($item.PSIsContainer) {
            if ($item.Name -notin $allowedTestDirs) {
                $violations += "G4: Unauthorized test directory: tests/contracts/$($item.Name)/"
            }
        } else {
            if ($item.Name -notin $allowedTestRootFiles) {
                $violations += "G5: Unauthorized test file at root: tests/contracts/$($item.Name)"
            }
        }
    }
} else {
    $violations += "G4: Missing directory: tests/contracts/"
}

$composabilityTest = Join-Path $testsRoot "composability\public_header_composability_tests.cpp"
if (-not (Test-Path $composabilityTest)) {
    $violations += "G6: Missing composability test: tests/contracts/composability/public_header_composability_tests.cpp"
}

Test-BucketDirectoryNames -RootPath $contractRoot -Label "G7: Production"
Test-BucketDirectoryNames -RootPath $testsRoot -Label "G8: Test"

Write-Host "--- Checks ---"
Write-Host "Production root files and family dirs"
Write-Host "Test root files and family dirs"
Write-Host "Composability test registration path"
Write-Host "Bucket directory names"
Write-Host ""

if ($violations.Count -eq 0) {
    Write-Host "PASS"
    Write-Host "Contract genealogy follows current family tree."
    exit 0
}

Write-Host "FAIL"
Write-Host "Violations found ($($violations.Count)):"
foreach ($v in $violations) {
    Write-Host "  - $v"
}
exit 1
