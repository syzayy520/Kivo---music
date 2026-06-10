// =============================================================================
// KivoAudioCoreCpp — seek_policy_tests.cpp
// Tests for seek policy enum contract types
// =============================================================================

#include "kivo/core/contract/seek/seek_anti_glitch_policy.hpp"
#include "kivo/core/contract/seek/stale_frame_discard_rule.hpp"
#include "../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// Tests
// =============================================================================
static void seek_anti_glitch_policy_values() {
    // Verify all enum values are distinct
    ASSERT(SeekAntiGlitchPolicy::Unknown != SeekAntiGlitchPolicy::MuteDuringSeek);
    ASSERT(SeekAntiGlitchPolicy::MuteDuringSeek != SeekAntiGlitchPolicy::FadeOutFadeIn);
    ASSERT(SeekAntiGlitchPolicy::FadeOutFadeIn != SeekAntiGlitchPolicy::Crossfade);
    ASSERT(SeekAntiGlitchPolicy::Crossfade != SeekAntiGlitchPolicy::None);
}

static void seek_anti_glitch_policy_default_is_unknown() {
    SeekAntiGlitchPolicy policy{};
    ASSERT(policy == SeekAntiGlitchPolicy::Unknown);
}

static void stale_frame_discard_rule_values() {
    // Verify all enum values are distinct
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
void run_seek_policy_tests(ContractTestRunner& runner) {
    std::cout << "--- Seek Policy Enums ---\n";
    runner.run("seek_anti_glitch_policy_values", seek_anti_glitch_policy_values);
    runner.run("seek_anti_glitch_policy_default_is_unknown", seek_anti_glitch_policy_default_is_unknown);
    runner.run("stale_frame_discard_rule_values", stale_frame_discard_rule_values);
    runner.run("stale_frame_discard_rule_default_is_unknown", stale_frame_discard_rule_default_is_unknown);
    std::cout << "\n";
}
