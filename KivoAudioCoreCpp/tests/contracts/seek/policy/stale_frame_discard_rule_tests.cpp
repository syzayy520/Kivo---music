// =============================================================================
// KivoAudioCoreCpp — stale_frame_discard_rule_tests.cpp
// Tests for StaleFrameDiscardRule enum
// =============================================================================

#include "kivo/core/contract/seek/policy/stale_frame_discard_rule.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// Tests
// =============================================================================
static void stale_frame_discard_rule_values() {
    ASSERT(StaleFrameDiscardRule::Unknown != StaleFrameDiscardRule::DiscardAll);
    ASSERT(StaleFrameDiscardRule::DiscardAll != StaleFrameDiscardRule::DiscardAfterSeek);
    ASSERT(StaleFrameDiscardRule::DiscardAfterSeek != StaleFrameDiscardRule::KeepIfDecoded);
}

static void stale_frame_discard_rule_default_is_unknown() {
    StaleFrameDiscardRule rule{};
    ASSERT(rule == StaleFrameDiscardRule::Unknown);
}

// =============================================================================
// Runner
// =============================================================================
void run_stale_frame_discard_rule_tests(ContractTestRunner& runner) {
    std::cout << "--- StaleFrameDiscardRule ---\n";
    runner.run("stale_frame_discard_rule_values", stale_frame_discard_rule_values);
    runner.run("stale_frame_discard_rule_default_is_unknown", stale_frame_discard_rule_default_is_unknown);
    std::cout << "\n";
}
