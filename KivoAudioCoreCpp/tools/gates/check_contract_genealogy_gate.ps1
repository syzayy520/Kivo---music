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
    "capability",
    "output",
    "realtime",
    "generation",
    "buffer",
    "queue",
    "source",
    "cue",
    "error",
    "state"
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
# Rule G3f: Output family — subdirectory tree with exact allowlists
# =============================================================================
$outputSubdirAllowlists = @{
    "truth"       = @("output_truth_snapshot.hpp", "bitperfect_truth_report.hpp", "bitperfect_rejection_reason.hpp")
    "processing"  = @("processing_participation.hpp")
    "path"        = @("device_format_match.hpp", "sample_transparency.hpp", "host_audio_engine_participation.hpp", "output_path_truth.hpp")
    "decision"    = @("output_policy_decision.hpp", "output_policy_explanation.hpp")
}

$outputDir = Join-Path $contractRoot "output"
if (Test-Path $outputDir) {
    $rootHpp = Get-ChildItem -Path $outputDir -Filter "*.hpp"
    if ($rootHpp.Count -gt 0) {
        foreach ($f in $rootHpp) {
            $violations += "G3f: Production file at output/ root (should be in subdirectory): $($f.Name)"
        }
    }

    foreach ($subdir in $outputSubdirAllowlists.Keys) {
        $subdirPath = Join-Path $outputDir $subdir
        if (Test-Path $subdirPath) {
            $actualFiles = Get-ChildItem -Path $subdirPath -Filter "*.hpp" | ForEach-Object { $_.Name }
            $expectedFiles = $outputSubdirAllowlists[$subdir]

            foreach ($f in $actualFiles) {
                if ($f -notin $expectedFiles) {
                    $violations += "G3f: Unexpected file in output/$subdir/: $f"
                }
            }
            foreach ($f in $expectedFiles) {
                if ($f -notin $actualFiles) {
                    $violations += "G3f: Missing file in output/$subdir/: $f"
                }
            }
        } else {
            $violations += "G3f: Missing output subdirectory: output/$subdir/"
        }
    }
} else {
    $violations += "G3f: Missing production family directory: include/kivo/core/contract/output/"
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
    "capability",
    "output",
    "realtime",
    "generation",
    "buffer",
    "queue",
    "source",
    "cue",
    "error",
    "state"
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
# Rule G6d: Output test family — subdirectory tree with exact allowlists
# =============================================================================
$outputTestSubdirAllowlists = @{
    "truth"       = @("output_truth_snapshot_tests.cpp", "bitperfect_truth_report_tests.cpp", "bitperfect_rejection_reason_tests.cpp")
    "processing"  = @("processing_participation_tests.cpp")
    "path"        = @("device_format_match_tests.cpp", "sample_transparency_tests.cpp", "host_audio_engine_participation_tests.cpp", "output_path_truth_tests.cpp")
    "decision"    = @("output_policy_decision_tests.cpp", "output_policy_explanation_tests.cpp")
}

$outputTestDir = Join-Path $testsRoot "output"
if (Test-Path $outputTestDir) {
    $rootCpp = Get-ChildItem -Path $outputTestDir -Filter "*.cpp"
    if ($rootCpp.Count -gt 0) {
        foreach ($f in $rootCpp) {
            $violations += "G6d: Test file at output/ root (should be in subdirectory): $($f.Name)"
        }
    }

    foreach ($subdir in $outputTestSubdirAllowlists.Keys) {
        $subdirPath = Join-Path $outputTestDir $subdir
        if (Test-Path $subdirPath) {
            $actualFiles = Get-ChildItem -Path $subdirPath -Filter "*.cpp" | ForEach-Object { $_.Name }
            $expectedFiles = $outputTestSubdirAllowlists[$subdir]

            foreach ($f in $actualFiles) {
                if ($f -notin $expectedFiles) {
                    $violations += "G6d: Unexpected test file in output/$subdir/: $f"
                }
            }
            foreach ($f in $expectedFiles) {
                if ($f -notin $actualFiles) {
                    $violations += "G6d: Missing test file in output/$subdir/: $f"
                }
            }
        } else {
            $violations += "G6d: Missing output test subdirectory: output/$subdir/"
        }
    }
} else {
    $violations += "G6d: Missing test family directory: tests/contracts/output/"
}

# =============================================================================
# Rule G3g: Realtime family — subdirectory tree with exact allowlists
# =============================================================================
$realtimeSubdirAllowlists = @{
    "path"        = @("realtime_path.hpp", "realtime_path_context.hpp")
    "prohibition" = @("realtime_allocation_rule.hpp", "realtime_blocking_io_rule.hpp", "realtime_locking_rule.hpp", "realtime_logging_rule.hpp", "realtime_callback_rule.hpp", "realtime_exception_rule.hpp")
    "ownership"   = @("ownership_transfer_rule.hpp", "ownership_visibility_rule.hpp")
    "buffer"      = @("preallocated_buffer_rule.hpp", "buffer_ownership_rule.hpp", "buffer_lifetime_rule.hpp")
    "transport"   = @("bounded_transport_rule.hpp", "spsc_semantics_rule.hpp")
    "boundary"    = @("realtime_boundary_contract.hpp", "non_realtime_boundary_contract.hpp")
}

$realtimeDir = Join-Path $contractRoot "realtime"
if (Test-Path $realtimeDir) {
    # Root must be empty of .hpp files (all in subdirectories)
    $rootHpp = Get-ChildItem -Path $realtimeDir -Filter "*.hpp"
    if ($rootHpp.Count -gt 0) {
        foreach ($f in $rootHpp) {
            $violations += "G3g: Production file at realtime/ root (should be in subdirectory): $($f.Name)"
        }
    }

    # Validate each subdirectory
    foreach ($subdir in $realtimeSubdirAllowlists.Keys) {
        $subdirPath = Join-Path $realtimeDir $subdir
        if (Test-Path $subdirPath) {
            $actualFiles = Get-ChildItem -Path $subdirPath -Filter "*.hpp" | ForEach-Object { $_.Name }
            $expectedFiles = $realtimeSubdirAllowlists[$subdir]

            foreach ($f in $actualFiles) {
                if ($f -notin $expectedFiles) {
                    $violations += "G3g: Unexpected file in realtime/$subdir/: $f"
                }
            }
            foreach ($f in $expectedFiles) {
                if ($f -notin $actualFiles) {
                    $violations += "G3g: Missing file in realtime/$subdir/: $f"
                }
            }
        } else {
            $violations += "G3g: Missing realtime subdirectory: realtime/$subdir/"
        }
    }
} else {
    $violations += "G3g: Missing production family directory: include/kivo/core/contract/realtime/"
}

# =============================================================================
# Rule G6e: Realtime test family — subdirectory tree with exact allowlists
# =============================================================================
$realtimeTestSubdirAllowlists = @{
    "path"        = @("realtime_path_contract_tests.cpp")
    "prohibition" = @("realtime_prohibition_contract_tests.cpp")
    "ownership"   = @("realtime_ownership_contract_tests.cpp")
    "buffer"      = @("realtime_buffer_contract_tests.cpp")
    "transport"   = @("realtime_transport_contract_tests.cpp")
    "boundary"    = @("realtime_boundary_contract_tests.cpp")
}

$realtimeTestDir = Join-Path $testsRoot "realtime"
if (Test-Path $realtimeTestDir) {
    # Root must be empty of .cpp files (all in subdirectories)
    $rootCpp = Get-ChildItem -Path $realtimeTestDir -Filter "*.cpp"
    if ($rootCpp.Count -gt 0) {
        foreach ($f in $rootCpp) {
            $violations += "G6e: Test file at realtime/ root (should be in subdirectory): $($f.Name)"
        }
    }

    foreach ($subdir in $realtimeTestSubdirAllowlists.Keys) {
        $subdirPath = Join-Path $realtimeTestDir $subdir
        if (Test-Path $subdirPath) {
            $actualFiles = Get-ChildItem -Path $subdirPath -Filter "*.cpp" | ForEach-Object { $_.Name }
            $expectedFiles = $realtimeTestSubdirAllowlists[$subdir]

            foreach ($f in $actualFiles) {
                if ($f -notin $expectedFiles) {
                    $violations += "G6e: Unexpected test file in realtime/$subdir/: $f"
                }
            }
            foreach ($f in $expectedFiles) {
                if ($f -notin $actualFiles) {
                    $violations += "G6e: Missing test file in realtime/$subdir/: $f"
                }
            }
        } else {
            $violations += "G6e: Missing realtime test subdirectory: realtime/$subdir/"
        }
    }
} else {
    $violations += "G6e: Missing test family directory: tests/contracts/realtime/"
}

# =============================================================================
# Rule G3h: Generation family — subdirectory tree with exact allowlists
# =============================================================================
$generationSubdirAllowlists = @{
    "identity"       = @("stream_generation.hpp", "seek_generation.hpp", "flush_generation.hpp", "device_generation.hpp")
    # NOTE: seek_generation.hpp defines SeekGenerationId (not SeekGeneration)
    # to avoid ODR collision with seek/generation/seek_generation.hpp
    "comparison"     = @("generation_match_rule.hpp")
    "classification" = @("stale_generation_classification.hpp")
    "advance"        = @("generation_reset_reason.hpp", "generation_advance_rule.hpp")
}

$generationDir = Join-Path $contractRoot "generation"
if (Test-Path $generationDir) {
    # Root must be empty of .hpp files (all in subdirectories)
    $rootHpp = Get-ChildItem -Path $generationDir -Filter "*.hpp"
    if ($rootHpp.Count -gt 0) {
        foreach ($f in $rootHpp) {
            $violations += "G3h: Production file at generation/ root (should be in subdirectory): $($f.Name)"
        }
    }

    # Validate each subdirectory
    foreach ($subdir in $generationSubdirAllowlists.Keys) {
        $subdirPath = Join-Path $generationDir $subdir
        if (Test-Path $subdirPath) {
            $actualFiles = Get-ChildItem -Path $subdirPath -Filter "*.hpp" | ForEach-Object { $_.Name }
            $expectedFiles = $generationSubdirAllowlists[$subdir]

            foreach ($f in $actualFiles) {
                if ($f -notin $expectedFiles) {
                    $violations += "G3h: Unexpected file in generation/$subdir/: $f"
                }
            }
            foreach ($f in $expectedFiles) {
                if ($f -notin $actualFiles) {
                    $violations += "G3h: Missing file in generation/$subdir/: $f"
                }
            }
        } else {
            $violations += "G3h: Missing generation subdirectory: generation/$subdir/"
        }
    }
} else {
    $violations += "G3h: Missing production family directory: include/kivo/core/contract/generation/"
}

# =============================================================================
# Rule G3i: Buffer family — subdirectory tree with exact allowlists
# =============================================================================
$bufferSubdirAllowlists = @{
    "identity"   = @("buffer_id.hpp", "buffer_generation.hpp")
    "ownership"  = @("buffer_ownership_mode.hpp", "buffer_ownership_transfer.hpp")
    "lifetime"   = @("buffer_lifetime_proof.hpp", "buffer_reuse_policy.hpp")
    "reference"  = @("buffer_reference_type.hpp")
    "share"      = @("buffer_share_semantics.hpp")
}

$bufferDir = Join-Path $contractRoot "buffer"
if (Test-Path $bufferDir) {
    # Root must be empty of .hpp files (all in subdirectories)
    $rootHpp = Get-ChildItem -Path $bufferDir -Filter "*.hpp"
    if ($rootHpp.Count -gt 0) {
        foreach ($f in $rootHpp) {
            $violations += "G3i: Production file at buffer/ root (should be in subdirectory): $($f.Name)"
        }
    }

    # Validate each subdirectory
    foreach ($subdir in $bufferSubdirAllowlists.Keys) {
        $subdirPath = Join-Path $bufferDir $subdir
        if (Test-Path $subdirPath) {
            $actualFiles = Get-ChildItem -Path $subdirPath -Filter "*.hpp" | ForEach-Object { $_.Name }
            $expectedFiles = $bufferSubdirAllowlists[$subdir]

            foreach ($f in $actualFiles) {
                if ($f -notin $expectedFiles) {
                    $violations += "G3i: Unexpected file in buffer/$subdir/: $f"
                }
            }
            foreach ($f in $expectedFiles) {
                if ($f -notin $actualFiles) {
                    $violations += "G3i: Missing file in buffer/$subdir/: $f"
                }
            }
        } else {
            $violations += "G3i: Missing buffer subdirectory: buffer/$subdir/"
        }
    }
} else {
    $violations += "G3i: Missing production family directory: include/kivo/core/contract/buffer/"
}

# =============================================================================
# Rule G3j: Queue family — subdirectory tree with exact allowlists
# =============================================================================
$queueSubdirAllowlists = @{
    "capacity"   = @("queue_capacity_policy.hpp", "queue_overflow_policy.hpp")
    "slot"       = @("queue_slot_state.hpp", "queue_slot_generation.hpp")
    "staleness"  = @("queue_staleness_threshold.hpp", "queue_staleness_policy.hpp")
    "boundary"   = @("queue_boundary_mode.hpp")
}

$queueDir = Join-Path $contractRoot "queue"
if (Test-Path $queueDir) {
    # Root must be empty of .hpp files (all in subdirectories)
    $rootHpp = Get-ChildItem -Path $queueDir -Filter "*.hpp"
    if ($rootHpp.Count -gt 0) {
        foreach ($f in $rootHpp) {
            $violations += "G3j: Production file at queue/ root (should be in subdirectory): $($f.Name)"
        }
    }

    # Validate each subdirectory
    foreach ($subdir in $queueSubdirAllowlists.Keys) {
        $subdirPath = Join-Path $queueDir $subdir
        if (Test-Path $subdirPath) {
            $actualFiles = Get-ChildItem -Path $subdirPath -Filter "*.hpp" | ForEach-Object { $_.Name }
            $expectedFiles = $queueSubdirAllowlists[$subdir]

            foreach ($f in $actualFiles) {
                if ($f -notin $expectedFiles) {
                    $violations += "G3j: Unexpected file in queue/$subdir/: $f"
                }
            }
            foreach ($f in $expectedFiles) {
                if ($f -notin $actualFiles) {
                    $violations += "G3j: Missing file in queue/$subdir/: $f"
                }
            }
        } else {
            $violations += "G3j: Missing queue subdirectory: queue/$subdir/"
        }
    }
} else {
    $violations += "G3j: Missing production family directory: include/kivo/core/contract/queue/"
}

# =============================================================================
# Rule G6f: Generation test family — subdirectory tree with exact allowlists
# =============================================================================
$generationTestSubdirAllowlists = @{
    "identity"       = @("generation_identity_tests.cpp")
    "comparison"     = @("generation_comparison_tests.cpp")
    "classification" = @("generation_classification_tests.cpp")
    "advance"        = @("generation_advance_tests.cpp")
}

$generationTestDir = Join-Path $testsRoot "generation"
if (Test-Path $generationTestDir) {
    # Root must be empty of .cpp files (all in subdirectories)
    $rootCpp = Get-ChildItem -Path $generationTestDir -Filter "*.cpp"
    if ($rootCpp.Count -gt 0) {
        foreach ($f in $rootCpp) {
            $violations += "G6f: Test file at generation/ root (should be in subdirectory): $($f.Name)"
        }
    }

    foreach ($subdir in $generationTestSubdirAllowlists.Keys) {
        $subdirPath = Join-Path $generationTestDir $subdir
        if (Test-Path $subdirPath) {
            $actualFiles = Get-ChildItem -Path $subdirPath -Filter "*.cpp" | ForEach-Object { $_.Name }
            $expectedFiles = $generationTestSubdirAllowlists[$subdir]

            foreach ($f in $actualFiles) {
                if ($f -notin $expectedFiles) {
                    $violations += "G6f: Unexpected test file in generation/$subdir/: $f"
                }
            }
            foreach ($f in $expectedFiles) {
                if ($f -notin $actualFiles) {
                    $violations += "G6f: Missing test file in generation/$subdir/: $f"
                }
            }
        } else {
            $violations += "G6f: Missing generation test subdirectory: generation/$subdir/"
        }
    }
} else {
    $violations += "G6f: Missing test family directory: tests/contracts/generation/"
}

# =============================================================================
# Rule G6g: Buffer test family — subdirectory tree with exact allowlists
# =============================================================================
$bufferTestSubdirAllowlists = @{
    "identity"   = @("buffer_identity_tests.cpp")
    "ownership"  = @("buffer_ownership_tests.cpp")
    "lifetime"   = @("buffer_lifetime_tests.cpp")
    "reference"  = @("buffer_reference_tests.cpp")
    "share"      = @("buffer_share_tests.cpp")
}

$bufferTestDir = Join-Path $testsRoot "buffer"
if (Test-Path $bufferTestDir) {
    # Root must be empty of .cpp files (all in subdirectories)
    $rootCpp = Get-ChildItem -Path $bufferTestDir -Filter "*.cpp"
    if ($rootCpp.Count -gt 0) {
        foreach ($f in $rootCpp) {
            $violations += "G6g: Test file at buffer/ root (should be in subdirectory): $($f.Name)"
        }
    }

    foreach ($subdir in $bufferTestSubdirAllowlists.Keys) {
        $subdirPath = Join-Path $bufferTestDir $subdir
        if (Test-Path $subdirPath) {
            $actualFiles = Get-ChildItem -Path $subdirPath -Filter "*.cpp" | ForEach-Object { $_.Name }
            $expectedFiles = $bufferTestSubdirAllowlists[$subdir]

            foreach ($f in $actualFiles) {
                if ($f -notin $expectedFiles) {
                    $violations += "G6g: Unexpected test file in buffer/$subdir/: $f"
                }
            }
            foreach ($f in $expectedFiles) {
                if ($f -notin $actualFiles) {
                    $violations += "G6g: Missing test file in buffer/$subdir/: $f"
                }
            }
        } else {
            $violations += "G6g: Missing buffer test subdirectory: buffer/$subdir/"
        }
    }
} else {
    $violations += "G6g: Missing test family directory: tests/contracts/buffer/"
}

# =============================================================================
# Rule G6h: Queue test family — subdirectory tree with exact allowlists
# =============================================================================
$queueTestSubdirAllowlists = @{
    "capacity"   = @("queue_capacity_tests.cpp")
    "slot"       = @("queue_slot_tests.cpp")
    "staleness"  = @("queue_staleness_tests.cpp")
    "boundary"   = @("queue_boundary_tests.cpp")
}

$queueTestDir = Join-Path $testsRoot "queue"
if (Test-Path $queueTestDir) {
    # Root must be empty of .cpp files (all in subdirectories)
    $rootCpp = Get-ChildItem -Path $queueTestDir -Filter "*.cpp"
    if ($rootCpp.Count -gt 0) {
        foreach ($f in $rootCpp) {
            $violations += "G6h: Test file at queue/ root (should be in subdirectory): $($f.Name)"
        }
    }

    foreach ($subdir in $queueTestSubdirAllowlists.Keys) {
        $subdirPath = Join-Path $queueTestDir $subdir
        if (Test-Path $subdirPath) {
            $actualFiles = Get-ChildItem -Path $subdirPath -Filter "*.cpp" | ForEach-Object { $_.Name }
            $expectedFiles = $queueTestSubdirAllowlists[$subdir]

            foreach ($f in $actualFiles) {
                if ($f -notin $expectedFiles) {
                    $violations += "G6h: Unexpected test file in queue/$subdir/: $f"
                }
            }
            foreach ($f in $expectedFiles) {
                if ($f -notin $actualFiles) {
                    $violations += "G6h: Missing test file in queue/$subdir/: $f"
                }
            }
        } else {
            $violations += "G6h: Missing queue test subdirectory: queue/$subdir/"
        }
    }
} else {
    $violations += "G6h: Missing test family directory: tests/contracts/queue/"
}

# =============================================================================
# Rule G3k: Source family — subdirectory tree with exact allowlists
# =============================================================================
$sourceSubdirAllowlists = @{
    "identity"  = @("source_identity.hpp", "source_generation.hpp", "source_location_ref.hpp")
    "location"  = @("source_uri.hpp", "source_path.hpp", "source_locator.hpp")
    "access"    = @("source_access_type.hpp", "source_readiness.hpp", "source_ownership_policy.hpp", "source_fault_policy.hpp")
    "reader"    = @("reader_identity.hpp", "reader_capability.hpp", "reader_boundary_contract.hpp", "reader_failure.hpp")
    "container" = @("container_identity.hpp", "container_boundary_contract.hpp", "container_capability.hpp", "container_failure.hpp")
    "boundary"  = @("source_boundary_contract.hpp", "source_boundary_mode.hpp")
    "outcome"   = @("source_outcome.hpp", "source_outcome_reason.hpp", "source_recovery_policy.hpp")
}

$sourceDir = Join-Path $contractRoot "source"
if (Test-Path $sourceDir) {
    # Root must be empty of .hpp files (all in subdirectories)
    $rootHpp = Get-ChildItem -Path $sourceDir -Filter "*.hpp"
    if ($rootHpp.Count -gt 0) {
        foreach ($f in $rootHpp) {
            $violations += "G3k: Production file at source/ root (should be in subdirectory): $($f.Name)"
        }
    }

    # Validate each subdirectory
    foreach ($subdir in $sourceSubdirAllowlists.Keys) {
        $subdirPath = Join-Path $sourceDir $subdir
        if (Test-Path $subdirPath) {
            $actualFiles = Get-ChildItem -Path $subdirPath -Filter "*.hpp" | ForEach-Object { $_.Name }
            $expectedFiles = $sourceSubdirAllowlists[$subdir]

            foreach ($f in $actualFiles) {
                if ($f -notin $expectedFiles) {
                    $violations += "G3k: Unexpected file in source/$subdir/: $f"
                }
            }
            foreach ($f in $expectedFiles) {
                if ($f -notin $actualFiles) {
                    $violations += "G3k: Missing file in source/$subdir/: $f"
                }
            }
        } else {
            $violations += "G3k: Missing source subdirectory: source/$subdir/"
        }
    }
} else {
    $violations += "G3k: Missing production family directory: include/kivo/core/contract/source/"
}

# =============================================================================
# Rule G6i: Source test family — subdirectory tree with exact allowlists
# =============================================================================
$sourceTestSubdirAllowlists = @{
    "identity"  = @("source_identity_tests.cpp")
    "location"  = @("source_location_tests.cpp")
    "access"    = @("source_access_tests.cpp")
    "reader"    = @("source_reader_tests.cpp")
    "container" = @("source_container_tests.cpp")
    "boundary"  = @("source_boundary_tests.cpp")
    "outcome"   = @("source_outcome_tests.cpp")
}

$sourceTestDir = Join-Path $testsRoot "source"
if (Test-Path $sourceTestDir) {
    # Root must be empty of .cpp files (all in subdirectories)
    $rootCpp = Get-ChildItem -Path $sourceTestDir -Filter "*.cpp"
    if ($rootCpp.Count -gt 0) {
        foreach ($f in $rootCpp) {
            $violations += "G6i: Test file at source/ root (should be in subdirectory): $($f.Name)"
        }
    }

    foreach ($subdir in $sourceTestSubdirAllowlists.Keys) {
        $subdirPath = Join-Path $sourceTestDir $subdir
        if (Test-Path $subdirPath) {
            $actualFiles = Get-ChildItem -Path $subdirPath -Filter "*.cpp" | ForEach-Object { $_.Name }
            $expectedFiles = $sourceTestSubdirAllowlists[$subdir]

            foreach ($f in $actualFiles) {
                if ($f -notin $expectedFiles) {
                    $violations += "G6i: Unexpected test file in source/$subdir/: $f"
                }
            }
            foreach ($f in $expectedFiles) {
                if ($f -notin $actualFiles) {
                    $violations += "G6i: Missing test file in source/$subdir/: $f"
                }
            }
        } else {
            $violations += "G6i: Missing source test subdirectory: source/$subdir/"
        }
    }
} else {
    $violations += "G6i: Missing test family directory: tests/contracts/source/"
}

# =============================================================================
# Rule G3l: Cue family — subdirectory tree with exact allowlists
# =============================================================================
$cueSubdirAllowlists = @{
    "boundary"   = @("cue_sheet_boundary.hpp", "whole_album_file.hpp", "virtual_track_segment.hpp")
    "range"      = @("source_time_range.hpp", "timeline_range.hpp", "track_segment_range.hpp")
    "continuity" = @("gapless_metadata.hpp", "cross_track_clock_continuity.hpp", "codec_delay.hpp", "padding.hpp", "preroll.hpp", "prefetch.hpp")
}

$cueDir = Join-Path $contractRoot "cue"
if (Test-Path $cueDir) {
    # Root must be empty of .hpp files (all in subdirectories)
    $rootHpp = Get-ChildItem -Path $cueDir -Filter "*.hpp"
    if ($rootHpp.Count -gt 0) {
        foreach ($f in $rootHpp) {
            $violations += "G3l: Production file at cue/ root (should be in subdirectory): $($f.Name)"
        }
    }

    # Validate each subdirectory
    foreach ($subdir in $cueSubdirAllowlists.Keys) {
        $subdirPath = Join-Path $cueDir $subdir
        if (Test-Path $subdirPath) {
            $actualFiles = Get-ChildItem -Path $subdirPath -Filter "*.hpp" | ForEach-Object { $_.Name }
            $expectedFiles = $cueSubdirAllowlists[$subdir]

            foreach ($f in $actualFiles) {
                if ($f -notin $expectedFiles) {
                    $violations += "G3l: Unexpected file in cue/$subdir/: $f"
                }
            }
            foreach ($f in $expectedFiles) {
                if ($f -notin $actualFiles) {
                    $violations += "G3l: Missing file in cue/$subdir/: $f"
                }
            }
        } else {
            $violations += "G3l: Missing cue subdirectory: cue/$subdir/"
        }
    }
} else {
    $violations += "G3l: Missing production family directory: include/kivo/core/contract/cue/"
}

# =============================================================================
# Rule G6j: Cue test family — subdirectory tree with exact allowlists
# =============================================================================
$cueTestSubdirAllowlists = @{
    "boundary"   = @("cue_boundary_tests.cpp")
    "range"      = @("cue_range_tests.cpp")
    "continuity" = @("cue_continuity_tests.cpp")
}

$cueTestDir = Join-Path $testsRoot "cue"
if (Test-Path $cueTestDir) {
    # Root must be empty of .cpp files (all in subdirectories)
    $rootCpp = Get-ChildItem -Path $cueTestDir -Filter "*.cpp"
    if ($rootCpp.Count -gt 0) {
        foreach ($f in $rootCpp) {
            $violations += "G6j: Test file at cue/ root (should be in subdirectory): $($f.Name)"
        }
    }

    foreach ($subdir in $cueTestSubdirAllowlists.Keys) {
        $subdirPath = Join-Path $cueTestDir $subdir
        if (Test-Path $subdirPath) {
            $actualFiles = Get-ChildItem -Path $subdirPath -Filter "*.cpp" | ForEach-Object { $_.Name }
            $expectedFiles = $cueTestSubdirAllowlists[$subdir]

            foreach ($f in $actualFiles) {
                if ($f -notin $expectedFiles) {
                    $violations += "G6j: Unexpected test file in cue/$subdir/: $f"
                }
            }
            foreach ($f in $expectedFiles) {
                if ($f -notin $actualFiles) {
                    $violations += "G6j: Missing test file in cue/$subdir/: $f"
                }
            }
        } else {
            $violations += "G6j: Missing cue test subdirectory: cue/$subdir/"
        }
    }
} else {
    $violations += "G6j: Missing test family directory: tests/contracts/cue/"
}

# =============================================================================
# Rule G3m: Error family — subdirectory tree with exact allowlists
# =============================================================================
$errorSubdirAllowlists = @{
    "domain"   = @("error_domain.hpp", "error_domain_category.hpp")
    "severity" = @("error_severity.hpp", "safe_cleanup_path.hpp", "error_attributes.hpp")
    "recovery" = @("recovery_action.hpp", "recovery_matrix.hpp")
    "outcome"  = @("error_outcome.hpp")
}

$errorDir = Join-Path $contractRoot "error"
if (Test-Path $errorDir) {
    # Root must be empty of .hpp files (all in subdirectories)
    $rootHpp = Get-ChildItem -Path $errorDir -Filter "*.hpp"
    if ($rootHpp.Count -gt 0) {
        foreach ($f in $rootHpp) {
            $violations += "G3m: Production file at error/ root (should be in subdirectory): $($f.Name)"
        }
    }

    # Validate each subdirectory
    foreach ($subdir in $errorSubdirAllowlists.Keys) {
        $subdirPath = Join-Path $errorDir $subdir
        if (Test-Path $subdirPath) {
            $actualFiles = Get-ChildItem -Path $subdirPath -Filter "*.hpp" | ForEach-Object { $_.Name }
            $expectedFiles = $errorSubdirAllowlists[$subdir]

            foreach ($f in $actualFiles) {
                if ($f -notin $expectedFiles) {
                    $violations += "G3m: Unexpected file in error/$subdir/: $f"
                }
            }
            foreach ($f in $expectedFiles) {
                if ($f -notin $actualFiles) {
                    $violations += "G3m: Missing file in error/$subdir/: $f"
                }
            }
        } else {
            $violations += "G3m: Missing error subdirectory: error/$subdir/"
        }
    }
} else {
    $violations += "G3m: Missing production family directory: include/kivo/core/contract/error/"
}

# =============================================================================
# Rule G3n: State family — subdirectory tree with exact allowlists
# =============================================================================
$stateSubdirAllowlists = @{
    "phase"      = @("core_state.hpp", "state_stability.hpp", "state_terminality.hpp")
    "transition" = @("state_transition_intent.hpp", "state_transition.hpp", "state_transition_validity.hpp", "state_transition_decision.hpp")
    "rejection"  = @("state_transition_rejection_reason.hpp", "illegal_transition_rule.hpp")
    "rule"       = @("reentrant_transition_policy.hpp", "transition_preemption_policy.hpp", "terminal_state_rule.hpp")
    "scenario"   = @("seeking_reentry_rule.hpp", "recovering_seek_rule.hpp", "draining_pause_rule.hpp", "failed_close_rule.hpp", "closed_mutation_rule.hpp", "device_lost_during_seek_rule.hpp", "shutdown_during_drain_rule.hpp")
    "contract"   = @("state_machine_contract.hpp")
}

$stateDir = Join-Path $contractRoot "state"
if (Test-Path $stateDir) {
    # Root must be empty of .hpp files (all in subdirectories)
    $rootHpp = Get-ChildItem -Path $stateDir -Filter "*.hpp"
    if ($rootHpp.Count -gt 0) {
        foreach ($f in $rootHpp) {
            $violations += "G3n: Production file at state/ root (should be in subdirectory): $($f.Name)"
        }
    }

    # Validate each subdirectory
    foreach ($subdir in $stateSubdirAllowlists.Keys) {
        $subdirPath = Join-Path $stateDir $subdir
        if (Test-Path $subdirPath) {
            $actualFiles = Get-ChildItem -Path $subdirPath -Filter "*.hpp" | ForEach-Object { $_.Name }
            $expectedFiles = $stateSubdirAllowlists[$subdir]

            foreach ($f in $actualFiles) {
                if ($f -notin $expectedFiles) {
                    $violations += "G3n: Unexpected file in state/$subdir/: $f"
                }
            }
            foreach ($f in $expectedFiles) {
                if ($f -notin $actualFiles) {
                    $violations += "G3n: Missing file in state/$subdir/: $f"
                }
            }
        } else {
            $violations += "G3n: Missing state subdirectory: state/$subdir/"
        }
    }
} else {
    $violations += "G3n: Missing production family directory: include/kivo/core/contract/state/"
}

# =============================================================================
# Rule G6k: Error test family — subdirectory tree with exact allowlists
# =============================================================================
$errorTestSubdirAllowlists = @{
    "domain"   = @("error_domain_tests.cpp", "error_domain_category_tests.cpp")
    "severity" = @("error_severity_tests.cpp", "error_attributes_tests.cpp")
    "recovery" = @("recovery_action_tests.cpp", "recovery_matrix_tests.cpp")
    "outcome"  = @("error_outcome_tests.cpp")
}

$errorTestDir = Join-Path $testsRoot "error"
if (Test-Path $errorTestDir) {
    # Root must be empty of .cpp files (all in subdirectories)
    $rootCpp = Get-ChildItem -Path $errorTestDir -Filter "*.cpp"
    if ($rootCpp.Count -gt 0) {
        foreach ($f in $rootCpp) {
            $violations += "G6k: Test file at error/ root (should be in subdirectory): $($f.Name)"
        }
    }

    foreach ($subdir in $errorTestSubdirAllowlists.Keys) {
        $subdirPath = Join-Path $errorTestDir $subdir
        if (Test-Path $subdirPath) {
            $actualFiles = Get-ChildItem -Path $subdirPath -Filter "*.cpp" | ForEach-Object { $_.Name }
            $expectedFiles = $errorTestSubdirAllowlists[$subdir]

            foreach ($f in $actualFiles) {
                if ($f -notin $expectedFiles) {
                    $violations += "G6k: Unexpected test file in error/$subdir/: $f"
                }
            }
            foreach ($f in $expectedFiles) {
                if ($f -notin $actualFiles) {
                    $violations += "G6k: Missing test file in error/$subdir/: $f"
                }
            }
        } else {
            $violations += "G6k: Missing error test subdirectory: error/$subdir/"
        }
    }
} else {
    $violations += "G6k: Missing test family directory: tests/contracts/error/"
}

# =============================================================================
# Rule G6l: State test family — subdirectory tree with exact allowlists
# =============================================================================
$stateTestSubdirAllowlists = @{
    "phase"      = @("state_phase_tests.cpp")
    "transition" = @("state_transition_tests.cpp")
    "rejection"  = @("state_rejection_tests.cpp")
    "rule"       = @("state_rule_tests.cpp")
    "scenario"   = @("state_scenario_tests.cpp")
    "contract"   = @("state_machine_contract_tests.cpp")
}

$stateTestDir = Join-Path $testsRoot "state"
if (Test-Path $stateTestDir) {
    # Root must be empty of .cpp files (all in subdirectories)
    $rootCpp = Get-ChildItem -Path $stateTestDir -Filter "*.cpp"
    if ($rootCpp.Count -gt 0) {
        foreach ($f in $rootCpp) {
            $violations += "G6l: Test file at state/ root (should be in subdirectory): $($f.Name)"
        }
    }

    foreach ($subdir in $stateTestSubdirAllowlists.Keys) {
        $subdirPath = Join-Path $stateTestDir $subdir
        if (Test-Path $subdirPath) {
            $actualFiles = Get-ChildItem -Path $subdirPath -Filter "*.cpp" | ForEach-Object { $_.Name }
            $expectedFiles = $stateTestSubdirAllowlists[$subdir]

            foreach ($f in $actualFiles) {
                if ($f -notin $expectedFiles) {
                    $violations += "G6l: Unexpected test file in state/$subdir/: $f"
                }
            }
            foreach ($f in $expectedFiles) {
                if ($f -notin $actualFiles) {
                    $violations += "G6l: Missing test file in state/$subdir/: $f"
                }
            }
        } else {
            $violations += "G6l: Missing state test subdirectory: state/$subdir/"
        }
    }
} else {
    $violations += "G6l: Missing test family directory: tests/contracts/state/"
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
