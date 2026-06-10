// =============================================================================
// KivoAudioCoreCpp — pause_resume_freeze_policy_tests.cpp
// Tests for PauseResumeFreezePolicy enum
// =============================================================================

#include "kivo/core/contract/clock/policy/pause_resume_freeze_policy.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// Tests
// =============================================================================
static void pause_resume_freeze_policy_values() {
    ASSERT(static_cast<uint8_t>(PauseResumeFreezePolicy::Unknown) == 0);
    ASSERT(PauseResumeFreezePolicy::Unknown != PauseResumeFreezePolicy::FreezeAll);
    ASSERT(PauseResumeFreezePolicy::FreezeAll != PauseResumeFreezePolicy::FreezeDevice);
    ASSERT(PauseResumeFreezePolicy::FreezeDevice != PauseResumeFreezePolicy::FreezeTimeline);
    ASSERT(PauseResumeFreezePolicy::FreezeTimeline != PauseResumeFreezePolicy::None);
}

// =============================================================================
// Runner
// =============================================================================
void run_pause_resume_freeze_policy_tests(ContractTestRunner& runner) {
    std::cout << "--- PauseResumeFreezePolicy ---\n";
    runner.run("pause_resume_freeze_policy_values", pause_resume_freeze_policy_values);
    std::cout << "\n";
}
