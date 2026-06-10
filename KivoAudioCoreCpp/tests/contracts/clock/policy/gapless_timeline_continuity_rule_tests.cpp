// =============================================================================
// KivoAudioCoreCpp — gapless_timeline_continuity_rule_tests.cpp
// Tests for GaplessTimelineContinuityRule enum
// =============================================================================

#include "kivo/core/contract/clock/policy/gapless_timeline_continuity_rule.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// Tests
// =============================================================================
static void gapless_timeline_continuity_rule_values() {
    ASSERT(static_cast<uint8_t>(GaplessTimelineContinuityRule::Unknown) == 0);
    ASSERT(GaplessTimelineContinuityRule::Unknown != GaplessTimelineContinuityRule::Continuous);
    ASSERT(GaplessTimelineContinuityRule::Continuous != GaplessTimelineContinuityRule::ResetPerTrack);
    ASSERT(GaplessTimelineContinuityRule::ResetPerTrack != GaplessTimelineContinuityRule::OffsetPerTrack);
}

// =============================================================================
// Runner
// =============================================================================
void run_gapless_timeline_continuity_rule_tests(ContractTestRunner& runner) {
    std::cout << "--- GaplessTimelineContinuityRule ---\n";
    runner.run("gapless_timeline_continuity_rule_values", gapless_timeline_continuity_rule_values);
    std::cout << "\n";
}
