# =============================================================================
# check_contract_genealogy_gate.ps1
# Contract Genealogy Gate — Flatness and Family Tree Enforcement
# =============================================================================
# Validates that include/kivo/core/contract/ and tests/contracts/ follow
# the genealogy tree structure. Prevents flatness regression.
# P0-012: capability/ composite family with exact allowlists.
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
    "seek",
    "capability"
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
# Rule G3: Single-file families contain exactly 1 .hpp with exact name
# Clock family expanded in P0-C-002
# =============================================================================
$singleFileFamilies = @{
    "format" = @("audio_format_descriptor.hpp")
}

# Seek family — multi-file (P0-C-003)
$seekFamilyFiles = @(
    "seek_flush.hpp",
    "seek_intent.hpp",
    "seek_target.hpp",
    "seek_generation.hpp",
    "seek_anti_glitch_policy.hpp",
    "stale_frame_discard_rule.hpp"
)

# Clock family — multi-file (P0-C-002)
$clockFamilyFiles = @(
    "clock_domain.hpp",
    "device_clock.hpp",
    "stream_clock.hpp",
    "timeline_clock.hpp",
    "decoded_position.hpp",
    "rendered_position.hpp",
    "drift_estimate.hpp",
    "pause_resume_freeze_policy.hpp",
    "device_lost_position_report_rule.hpp",
    "seek_clock_reset_rule.hpp",
    "drain_eos_timeline_rule.hpp",
    "gapless_timeline_continuity_rule.hpp"
)

foreach ($dir in $singleFileFamilies.Keys) {
    $dirPath = Join-Path $contractRoot $dir
    if (Test-Path $dirPath) {
        $hppFiles = Get-ChildItem -Path $dirPath -Filter "*.hpp"
        if ($hppFiles.Count -ne 1) {
            $violations += "G3: Production family '$dir' should contain exactly 1 .hpp file, found $($hppFiles.Count)"
        }
        foreach ($f in $hppFiles) {
            if ($f.Name -notin $singleFileFamilies[$dir]) {
                $violations += "G3: Unexpected file in production family '$dir': $($f.Name)"
            }
        }
    } else {
        $violations += "G3: Missing production family directory: include/kivo/core/contract/$dir/"
    }
}

# Validate clock family (multi-file)
$clockDir = Join-Path $contractRoot "clock"
if (Test-Path $clockDir) {
    $hppFiles = Get-ChildItem -Path $clockDir -Filter "*.hpp"
    foreach ($f in $hppFiles) {
        if ($f.Name -notin $clockFamilyFiles) {
            $violations += "G3: Unexpected file in production family 'clock': $($f.Name)"
        }
    }
    foreach ($f in $clockFamilyFiles) {
        if ($f -notin ($hppFiles | ForEach-Object { $_.Name })) {
            $violations += "G3: Missing file in production family 'clock': $f"
        }
    }
} else {
    $violations += "G3: Missing production family directory: include/kivo/core/contract/clock/"
}

# Validate seek family (multi-file)
$seekDir = Join-Path $contractRoot "seek"
if (Test-Path $seekDir) {
    $hppFiles = Get-ChildItem -Path $seekDir -Filter "*.hpp"
    foreach ($f in $hppFiles) {
        if ($f.Name -notin $seekFamilyFiles) {
            $violations += "G3: Unexpected file in production family 'seek': $($f.Name)"
        }
    }
    foreach ($f in $seekFamilyFiles) {
        if ($f -notin ($hppFiles | ForEach-Object { $_.Name })) {
            $violations += "G3: Missing file in production family 'seek': $f"
        }
    }
} else {
    $violations += "G3: Missing production family directory: include/kivo/core/contract/seek/"
}

# =============================================================================
# Rule G3b: Capability composite family — root items
# =============================================================================
$capabilityRoot = Join-Path $contractRoot "capability"

$allowedCapabilityRootItems = @(
    "mod.hpp",
    "identity",
    "quality",
    "constraint",
    "component",
    "domain",
    "negotiation"
)

if (Test-Path $capabilityRoot) {
    $items = Get-ChildItem -Path $capabilityRoot
    foreach ($item in $items) {
        if ($item.Name -notin $allowedCapabilityRootItems) {
            $violations += "G3b: Unauthorized item in capability/ root: $($item.Name)"
        }
    }

    # =========================================================================
    # Rule G3c: Each capability subdirectory has exact file allowlist
    # =========================================================================
    $capabilitySubdirAllowlists = @{
        "identity"    = @("scope.hpp", "subject.hpp", "source.hpp")
        "quality"     = @("level.hpp", "confidence.hpp", "path.hpp")
        "constraint"  = @("strictness.hpp", "memory.hpp", "buffer.hpp")
        "component"   = @("profile.hpp", "requirement.hpp", "hdr.hpp", "decode.hpp", "render.hpp", "output.hpp")
        "domain"      = @("source_caps.hpp", "clock.hpp", "thread.hpp", "conversion.hpp")
        "negotiation" = @("candidate.hpp", "candidate_presence.hpp", "result.hpp", "constraints.hpp", "comparison.hpp")
    }

    foreach ($subdir in $capabilitySubdirAllowlists.Keys) {
        $subdirPath = Join-Path $capabilityRoot $subdir
        if (Test-Path $subdirPath) {
            $actualFiles = Get-ChildItem -Path $subdirPath -Filter "*.hpp" | ForEach-Object { $_.Name }
            $expectedFiles = $capabilitySubdirAllowlists[$subdir]

            foreach ($f in $actualFiles) {
                if ($f -notin $expectedFiles) {
                    $violations += "G3c: Unexpected file in capability/$subdir/: $f"
                }
            }
            foreach ($f in $expectedFiles) {
                if ($f -notin $actualFiles) {
                    $violations += "G3c: Missing file in capability/$subdir/: $f"
                }
            }
        } else {
            $violations += "G3c: Missing capability subdirectory: capability/$subdir/"
        }
    }
} else {
    $violations += "G3b: Missing capability directory: include/kivo/core/contract/capability/"
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
    "seek",
    "capability"
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
    "clock"      = @("clock_domain_tests.cpp", "device_clock_tests.cpp", "stream_clock_tests.cpp", "timeline_clock_tests.cpp", "position_tests.cpp", "drift_estimate_tests.cpp", "clock_policy_tests.cpp")
    "seek"       = @("seek_flush_tests.cpp", "seek_intent_tests.cpp", "seek_target_tests.cpp", "seek_generation_tests.cpp", "seek_policy_tests.cpp")
    "capability" = @("capability_tests_main.cpp", "identity_tests.cpp", "quality_tests.cpp", "constraint_tests.cpp", "component_tests.cpp", "domain_tests.cpp", "negotiation_tests.cpp")
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
