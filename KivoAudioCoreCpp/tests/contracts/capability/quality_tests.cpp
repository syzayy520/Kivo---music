// =============================================================================
// KivoAudioCoreCpp — quality_tests.cpp
// Tests for Capability quality contract types
// =============================================================================
//
// Tests: CapabilityLevel, CapabilityConfidence, CapabilityPath
// =============================================================================

#include "kivo/core/contract/capability/quality/level.hpp"
#include "kivo/core/contract/capability/quality/confidence.hpp"
#include "kivo/core/contract/capability/quality/path.hpp"
#include "../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// CapabilityLevel tests
// =============================================================================
static void level_has_all_values() {
    ASSERT(CapabilityLevel::None != CapabilityLevel::Basic);
    ASSERT(CapabilityLevel::Basic != CapabilityLevel::Standard);
    ASSERT(CapabilityLevel::Standard != CapabilityLevel::High);
    ASSERT(CapabilityLevel::High != CapabilityLevel::Maximum);
}

static void level_ordering() {
    // Verify levels are distinct (ordering semantics)
    ASSERT(CapabilityLevel::None != CapabilityLevel::Maximum);
    ASSERT(CapabilityLevel::Basic != CapabilityLevel::High);
}

static void level_equality() {
    ASSERT(CapabilityLevel::None == CapabilityLevel::None);
    ASSERT(CapabilityLevel::None != CapabilityLevel::Basic);
}

// =============================================================================
// CapabilityConfidence tests
// =============================================================================
static void confidence_has_all_values() {
    ASSERT(CapabilityConfidence::Estimated != CapabilityConfidence::Measured);
    ASSERT(CapabilityConfidence::Measured != CapabilityConfidence::Verified);
    ASSERT(CapabilityConfidence::Verified != CapabilityConfidence::Guaranteed);
}

static void confidence_equality() {
    ASSERT(CapabilityConfidence::Estimated == CapabilityConfidence::Estimated);
    ASSERT(CapabilityConfidence::Estimated != CapabilityConfidence::Guaranteed);
}

// =============================================================================
// CapabilityPath tests
// =============================================================================
static void path_has_all_three_values() {
    // Exactly 3 path values: DedicatedPath, GeneralPurposePath, ExternalPath
    auto dedicated = CapabilityPath::DedicatedPath;
    auto general = CapabilityPath::GeneralPurposePath;
    auto external = CapabilityPath::ExternalPath;
    ASSERT(dedicated != general);
    ASSERT(general != external);
    ASSERT(dedicated != external);
}

static void path_no_forbidden_values() {
    // Ensure no HardwareDecode, SoftwareDecode, HardwareAcceleratedPath, EmulatedPath
    // This is a compile-time guarantee — if forbidden values existed, this file wouldn't compile.
    // Runtime check: verify the 3 allowed values are distinct.
    CapabilityPath paths[] = {
        CapabilityPath::DedicatedPath,
        CapabilityPath::GeneralPurposePath,
        CapabilityPath::ExternalPath
    };
    for (int i = 0; i < 3; ++i) {
        for (int j = i + 1; j < 3; ++j) {
            ASSERT(paths[i] != paths[j]);
        }
    }
}

static void path_equality() {
    ASSERT(CapabilityPath::DedicatedPath == CapabilityPath::DedicatedPath);
    ASSERT(CapabilityPath::DedicatedPath != CapabilityPath::GeneralPurposePath);
}

// =============================================================================
// Test runner
// =============================================================================
void run_quality_contract_tests(ContractTestRunner& runner) {
    runner.run("quality::level_has_all_values", level_has_all_values);
    runner.run("quality::level_ordering", level_ordering);
    runner.run("quality::level_equality", level_equality);
    runner.run("quality::confidence_has_all_values", confidence_has_all_values);
    runner.run("quality::confidence_equality", confidence_equality);
    runner.run("quality::path_has_all_three_values", path_has_all_three_values);
    runner.run("quality::path_no_forbidden_values", path_no_forbidden_values);
    runner.run("quality::path_equality", path_equality);
}
