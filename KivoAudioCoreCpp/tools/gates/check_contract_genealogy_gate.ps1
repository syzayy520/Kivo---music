# =============================================================================
# check_contract_genealogy_gate.ps1
# Contract Genealogy Gate — Flatness and Family Tree Enforcement
# =============================================================================
# Validates that include/kivo/core/contract/ and tests/contracts/ follow
# the genealogy tree structure. Prevents flatness regression.
# P0-012: capability/ composite family with exact allowlists.
# P0-C-002/003: clock/ and seek/ migrated to subdirectory trees.
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
# =============================================================================
# (format/ moved to G3e as multi-subdirectory family in P0-C-004)
$singleFileFamilies = @{
}

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

# =============================================================================
# Rule G3a: Clock family — subdirectory tree with exact allowlists
# =============================================================================
$clockSubdirAllowlists = @{
    "domain"   = @("clock_domain.hpp", "device_clock.hpp", "stream_clock.hpp", "timeline_clock.hpp")
    "position" = @("decoded_position.hpp", "rendered_position.hpp")
    "drift"    = @("drift_estimate.hpp")
    "policy"   = @("pause_resume_freeze_policy.hpp", "device_lost_position_report_rule.hpp", "seek_clock_reset_rule.hpp", "drain_eos_timeline_rule.hpp", "gapless_timeline_continuity_rule.hpp")
}

$clockDir = Join-Path $contractRoot "clock"
if (Test-Path $clockDir) {
    # Root must be empty of .hpp files (all in subdirectories)
    $rootHpp = Get-ChildItem -Path $clockDir -Filter "*.hpp"
    if ($rootHpp.Count -gt 0) {
        foreach ($f in $rootHpp) {
            $violations += "G3a: Production file at clock/ root (should be in subdirectory): $($f.Name)"
        }
    }

    # Validate each subdirectory
    foreach ($subdir in $clockSubdirAllowlists.Keys) {
        $subdirPath = Join-Path $clockDir $subdir
        if (Test-Path $subdirPath) {
            $actualFiles = Get-ChildItem -Path $subdirPath -Filter "*.hpp" | ForEach-Object { $_.Name }
            $expectedFiles = $clockSubdirAllowlists[$subdir]

            foreach ($f in $actualFiles) {
                if ($f -notin $expectedFiles) {
                    $violations += "G3a: Unexpected file in clock/$subdir/: $f"
                }
            }
            foreach ($f in $expectedFiles) {
                if ($f -notin $actualFiles) {
                    $violations += "G3a: Missing file in clock/$subdir/: $f"
                }
            }
        } else {
            $violations += "G3a: Missing clock subdirectory: clock/$subdir/"
        }
    }
} else {
    $violations += "G3a: Missing production family directory: include/kivo/core/contract/clock/"
}

# =============================================================================
# Rule G3b: Seek family — subdirectory tree with exact allowlists
# =============================================================================
$seekSubdirAllowlists = @{
    "flush"      = @("seek_flush.hpp")
    "intent"     = @("seek_intent.hpp")
    "target"     = @("seek_target.hpp")
    "generation" = @("seek_generation.hpp")
    "policy"     = @("seek_anti_glitch_policy.hpp", "stale_frame_discard_rule.hpp")
}

$seekDir = Join-Path $contractRoot "seek"
if (Test-Path $seekDir) {
    # Root must be empty of .hpp files (all in subdirectories)
    $rootHpp = Get-ChildItem -Path $seekDir -Filter "*.hpp"
    if ($rootHpp.Count -gt 0) {
        foreach ($f in $rootHpp) {
            $violations += "G3b: Production file at seek/ root (should be in subdirectory): $($f.Name)"
        }
    }

    # Validate each subdirectory
    foreach ($subdir in $seekSubdirAllowlists.Keys) {
        $subdirPath = Join-Path $seekDir $subdir
        if (Test-Path $subdirPath) {
            $actualFiles = Get-ChildItem -Path $subdirPath -Filter "*.hpp" | ForEach-Object { $_.Name }
            $expectedFiles = $seekSubdirAllowlists[$subdir]

            foreach ($f in $actualFiles) {
                if ($f -notin $expectedFiles) {
                    $violations += "G3b: Unexpected file in seek/$subdir/: $f"
                }
            }
            foreach ($f in $expectedFiles) {
                if ($f -notin $actualFiles) {
                    $violations += "G3b: Missing file in seek/$subdir/: $f"
                }
            }
        } else {
            $violations += "G3b: Missing seek subdirectory: seek/$subdir/"
        }
    }
} else {
    $violations += "G3b: Missing production family directory: include/kivo/core/contract/seek/"
}

# =============================================================================
# Rule G3c: Capability composite family — root items
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
            $violations += "G3c: Unauthorized item in capability/ root: $($item.Name)"
        }
    }

    # =========================================================================
    # Rule G3d: Each capability subdirectory has exact file allowlist
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
                    $violations += "G3d: Unexpected file in capability/$subdir/: $f"
                }
            }
            foreach ($f in $expectedFiles) {
                if ($f -notin $actualFiles) {
                    $violations += "G3d: Missing file in capability/$subdir/: $f"
                }
            }
        } else {
            $violations += "G3d: Missing capability subdirectory: capability/$subdir/"
        }
    }
} else {
    $violations += "G3c: Missing capability directory: include/kivo/core/contract/capability/"
}

# =============================================================================
# Rule G3e: Format family — subdirectory tree with exact allowlists
# =============================================================================
$formatSubdirAllowlists = @{
    "descriptor"   = @("audio_format_descriptor.hpp", "sample_format.hpp", "channel_layout.hpp", "frame_layout.hpp", "channel_mask.hpp")
    "roles"        = @("native_decoded_format.hpp", "core_canonical_format.hpp", "render_format.hpp", "device_format.hpp")
    "negotiation"  = @("format_negotiation_result.hpp", "conversion_policy.hpp", "resample_decision.hpp", "bit_perfect_eligibility.hpp", "negotiated_output_mode.hpp")
}

$formatDir = Join-Path $contractRoot "format"
if (Test-Path $formatDir) {
    # Root must be empty of .hpp files (all in subdirectories)
    $rootHpp = Get-ChildItem -Path $formatDir -Filter "*.hpp"
    if ($rootHpp.Count -gt 0) {
        foreach ($f in $rootHpp) {
            $violations += "G3e: Production file at format/ root (should be in subdirectory): $($f.Name)"
        }
    }

    # Validate each subdirectory
    foreach ($subdir in $formatSubdirAllowlists.Keys) {
        $subdirPath = Join-Path $formatDir $subdir
        if (Test-Path $subdirPath) {
            $actualFiles = Get-ChildItem -Path $subdirPath -Filter "*.hpp" | ForEach-Object { $_.Name }
            $expectedFiles = $formatSubdirAllowlists[$subdir]

            foreach ($f in $actualFiles) {
                if ($f -notin $expectedFiles) {
                    $violations += "G3e: Unexpected file in format/$subdir/: $f"
                }
            }
            foreach ($f in $expectedFiles) {
                if ($f -notin $actualFiles) {
                    $violations += "G3e: Missing file in format/$subdir/: $f"
                }
            }
        } else {
            $violations += "G3e: Missing format subdirectory: format/$subdir/"
        }
    }
} else {
    $violations += "G3e: Missing production family directory: include/kivo/core/contract/format/"
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
    "capability" = @("capability_tests_main.cpp", "identity_tests.cpp", "quality_tests.cpp", "constraint_tests.cpp", "component_tests.cpp", "domain_tests.cpp", "negotiation_tests.cpp")
}

# Single-file test families
foreach ($dir in $expectedTestFiles.Keys) {
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
# Rule G6a: Clock test family — subdirectory tree with exact allowlists
# =============================================================================
$clockTestSubdirAllowlists = @{
    "domain"   = @("clock_domain_tests.cpp", "device_clock_tests.cpp", "stream_clock_tests.cpp", "timeline_clock_tests.cpp")
    "position" = @("decoded_position_tests.cpp", "rendered_position_tests.cpp")
    "drift"    = @("drift_estimate_tests.cpp")
    "policy"   = @("pause_resume_freeze_policy_tests.cpp", "device_lost_position_report_rule_tests.cpp", "seek_clock_reset_rule_tests.cpp", "drain_eos_timeline_rule_tests.cpp", "gapless_timeline_continuity_rule_tests.cpp")
}

$clockTestDir = Join-Path $testsRoot "clock"
if (Test-Path $clockTestDir) {
    # Root must be empty of .cpp files (all in subdirectories)
    $rootCpp = Get-ChildItem -Path $clockTestDir -Filter "*.cpp"
    if ($rootCpp.Count -gt 0) {
        foreach ($f in $rootCpp) {
            $violations += "G6a: Test file at clock/ root (should be in subdirectory): $($f.Name)"
        }
    }

    foreach ($subdir in $clockTestSubdirAllowlists.Keys) {
        $subdirPath = Join-Path $clockTestDir $subdir
        if (Test-Path $subdirPath) {
            $actualFiles = Get-ChildItem -Path $subdirPath -Filter "*.cpp" | ForEach-Object { $_.Name }
            $expectedFiles = $clockTestSubdirAllowlists[$subdir]

            foreach ($f in $actualFiles) {
                if ($f -notin $expectedFiles) {
                    $violations += "G6a: Unexpected test file in clock/$subdir/: $f"
                }
            }
            foreach ($f in $expectedFiles) {
                if ($f -notin $actualFiles) {
                    $violations += "G6a: Missing test file in clock/$subdir/: $f"
                }
            }
        } else {
            $violations += "G6a: Missing clock test subdirectory: clock/$subdir/"
        }
    }
} else {
    $violations += "G6a: Missing test family directory: tests/contracts/clock/"
}

# =============================================================================
# Rule G6b: Seek test family — subdirectory tree with exact allowlists
# =============================================================================
$seekTestSubdirAllowlists = @{
    "flush"      = @("seek_flush_tests.cpp")
    "intent"     = @("seek_intent_tests.cpp")
    "target"     = @("seek_target_tests.cpp")
    "generation" = @("seek_generation_tests.cpp")
    "policy"     = @("seek_anti_glitch_policy_tests.cpp", "stale_frame_discard_rule_tests.cpp")
}

$seekTestDir = Join-Path $testsRoot "seek"
if (Test-Path $seekTestDir) {
    # Root must be empty of .cpp files (all in subdirectories)
    $rootCpp = Get-ChildItem -Path $seekTestDir -Filter "*.cpp"
    if ($rootCpp.Count -gt 0) {
        foreach ($f in $rootCpp) {
            $violations += "G6b: Test file at seek/ root (should be in subdirectory): $($f.Name)"
        }
    }

    foreach ($subdir in $seekTestSubdirAllowlists.Keys) {
        $subdirPath = Join-Path $seekTestDir $subdir
        if (Test-Path $subdirPath) {
            $actualFiles = Get-ChildItem -Path $subdirPath -Filter "*.cpp" | ForEach-Object { $_.Name }
            $expectedFiles = $seekTestSubdirAllowlists[$subdir]

            foreach ($f in $actualFiles) {
                if ($f -notin $expectedFiles) {
                    $violations += "G6b: Unexpected test file in seek/$subdir/: $f"
                }
            }
            foreach ($f in $expectedFiles) {
                if ($f -notin $actualFiles) {
                    $violations += "G6b: Missing test file in seek/$subdir/: $f"
                }
            }
        } else {
            $violations += "G6b: Missing seek test subdirectory: seek/$subdir/"
        }
    }
} else {
    $violations += "G6b: Missing test family directory: tests/contracts/seek/"
}

# =============================================================================
# Rule G6c: Format test family — subdirectory tree with exact allowlists
# =============================================================================
$formatTestSubdirAllowlists = @{
    "descriptor"   = @("audio_format_descriptor_tests.cpp", "sample_format_tests.cpp", "channel_layout_tests.cpp", "frame_layout_tests.cpp", "channel_mask_tests.cpp")
    "roles"        = @("native_decoded_format_tests.cpp", "core_canonical_format_tests.cpp", "render_format_tests.cpp", "device_format_tests.cpp")
    "negotiation"  = @("format_negotiation_result_tests.cpp", "conversion_policy_tests.cpp", "resample_decision_tests.cpp", "bit_perfect_eligibility_tests.cpp", "negotiated_output_mode_tests.cpp")
}

$formatTestDir = Join-Path $testsRoot "format"
if (Test-Path $formatTestDir) {
    # Root must be empty of .cpp files (all in subdirectories)
    $rootCpp = Get-ChildItem -Path $formatTestDir -Filter "*.cpp"
    if ($rootCpp.Count -gt 0) {
        foreach ($f in $rootCpp) {
            $violations += "G6c: Test file at format/ root (should be in subdirectory): $($f.Name)"
        }
    }

    foreach ($subdir in $formatTestSubdirAllowlists.Keys) {
        $subdirPath = Join-Path $formatTestDir $subdir
        if (Test-Path $subdirPath) {
            $actualFiles = Get-ChildItem -Path $subdirPath -Filter "*.cpp" | ForEach-Object { $_.Name }
            $expectedFiles = $formatTestSubdirAllowlists[$subdir]

            foreach ($f in $actualFiles) {
                if ($f -notin $expectedFiles) {
                    $violations += "G6c: Unexpected test file in format/$subdir/: $f"
                }
            }
            foreach ($f in $expectedFiles) {
                if ($f -notin $actualFiles) {
                    $violations += "G6c: Missing test file in format/$subdir/: $f"
                }
            }
        } else {
            $violations += "G6c: Missing format test subdirectory: format/$subdir/"
        }
    }
} else {
    $violations += "G6c: Missing test family directory: tests/contracts/format/"
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
