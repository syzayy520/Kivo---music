// =============================================================================
// KivoAudioCoreCpp — seek_anti_glitch_policy_tests.cpp
// Tests for SeekAntiGlitchPolicy enum
// =============================================================================

#include "kivo/core/contract/seek/policy/seek_anti_glitch_policy.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// Tests
// =============================================================================
static void seek_anti_glitch_policy_values() {
    ASSERT(SeekAntiGlitchPolicy::Unknown != SeekAntiGlitchPolicy::MuteDuringSeek);
    ASSERT(SeekAntiGlitchPolicy::MuteDuringSeek != SeekAntiGlitchPolicy::FadeOutFadeIn);
    ASSERT(SeekAntiGlitchPolicy::FadeOutFadeIn != SeekAntiGlitchPolicy::Crossfade);
    ASSERT(SeekAntiGlitchPolicy::Crossfade != SeekAntiGlitchPolicy::None);
}

static void seek_anti_glitch_policy_default_is_unknown() {
    SeekAntiGlitchPolicy policy{};
    ASSERT(policy == SeekAntiGlitchPolicy::Unknown);
}

// =============================================================================
// Runner
// =============================================================================
void run_seek_anti_glitch_policy_tests(ContractTestRunner& runner) {
    std::cout << "--- SeekAntiGlitchPolicy ---\n";
    runner.run("seek_anti_glitch_policy_values", seek_anti_glitch_policy_values);
    runner.run("seek_anti_glitch_policy_default_is_unknown", seek_anti_glitch_policy_default_is_unknown);
    std::cout << "\n";
}
