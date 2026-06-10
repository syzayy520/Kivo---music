// =============================================================================
// KivoAudioCoreCpp — drain_eos_timeline_rule_tests.cpp
// Tests for DrainEosTimelineRule enum
// =============================================================================

#include "kivo/core/contract/clock/policy/drain_eos_timeline_rule.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// Tests
// =============================================================================
static void drain_eos_timeline_rule_values() {
    ASSERT(static_cast<uint8_t>(DrainEosTimelineRule::Unknown) == 0);
    ASSERT(DrainEosTimelineRule::Unknown != DrainEosTimelineRule::HoldAtEnd);
    ASSERT(DrainEosTimelineRule::HoldAtEnd != DrainEosTimelineRule::ResetToStart);
    ASSERT(DrainEosTimelineRule::ResetToStart != DrainEosTimelineRule::ContinueRunning);
}

// =============================================================================
// Runner
// =============================================================================
void run_drain_eos_timeline_rule_tests(ContractTestRunner& runner) {
    std::cout << "--- DrainEosTimelineRule ---\n";
    runner.run("drain_eos_timeline_rule_values", drain_eos_timeline_rule_values);
    std::cout << "\n";
}
