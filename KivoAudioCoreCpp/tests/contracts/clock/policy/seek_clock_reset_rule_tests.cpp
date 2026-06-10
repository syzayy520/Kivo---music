// =============================================================================
// KivoAudioCoreCpp — seek_clock_reset_rule_tests.cpp
// Tests for SeekClockResetRule enum
// =============================================================================

#include "kivo/core/contract/clock/policy/seek_clock_reset_rule.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// Tests
// =============================================================================
static void seek_clock_reset_rule_values() {
    ASSERT(static_cast<uint8_t>(SeekClockResetRule::Unknown) == 0);
    ASSERT(SeekClockResetRule::Unknown != SeekClockResetRule::ResetAll);
    ASSERT(SeekClockResetRule::ResetAll != SeekClockResetRule::ResetDevice);
    ASSERT(SeekClockResetRule::ResetDevice != SeekClockResetRule::ResetStream);
    ASSERT(SeekClockResetRule::ResetStream != SeekClockResetRule::ResetTimeline);
    ASSERT(SeekClockResetRule::ResetTimeline != SeekClockResetRule::IncrementGeneration);
}

// =============================================================================
// Runner
// =============================================================================
void run_seek_clock_reset_rule_tests(ContractTestRunner& runner) {
    std::cout << "--- SeekClockResetRule ---\n";
    runner.run("seek_clock_reset_rule_values", seek_clock_reset_rule_values);
    std::cout << "\n";
}
