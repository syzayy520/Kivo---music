// =============================================================================
// KivoAudioCoreCpp — resample_decision_tests.cpp
// Tests for ResampleDecision contract type
// =============================================================================

#include "kivo/core/contract/format/negotiation/resample_decision.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void default_decision_is_not_needed() {
    ResampleDecision decision;
    ASSERT(!decision.resample);
    ASSERT(!decision.is_needed());
    ASSERT(decision.reason == ResampleReason::NotNeeded);
}

static void device_mismatch_requires_resample() {
    ResampleDecision decision;
    decision.resample = true;
    decision.reason = ResampleReason::DeviceMismatch;
    decision.source_rate = 44100;
    decision.target_rate = 48000;
    ASSERT(decision.is_needed());
    ASSERT(decision.source_rate == 44100);
    ASSERT(decision.target_rate == 48000);
}

static void forbidden_by_policy() {
    ResampleDecision decision;
    decision.resample = false;
    decision.reason = ResampleReason::ForbiddenByPolicy;
    ASSERT(!decision.is_needed());
    ASSERT(decision.reason == ResampleReason::ForbiddenByPolicy);
}

static void required_by_policy() {
    ResampleDecision decision;
    decision.resample = true;
    decision.reason = ResampleReason::RequiredByPolicy;
    ASSERT(decision.is_needed());
}

static void equality_same() {
    ResampleDecision a;
    a.resample = true;
    a.source_rate = 44100;
    a.target_rate = 48000;

    ResampleDecision b = a;
    ASSERT(a == b);
}

static void equality_different() {
    ResampleDecision a;
    a.source_rate = 44100;

    ResampleDecision b;
    b.source_rate = 48000;

    ASSERT(!(a == b));
}

void run_resample_decision_contract_tests(ContractTestRunner& runner) {
    std::cout << "--- ResampleDecision ---\n";
    runner.run("default_decision_is_not_needed", default_decision_is_not_needed);
    runner.run("device_mismatch_requires_resample", device_mismatch_requires_resample);
    runner.run("forbidden_by_policy", forbidden_by_policy);
    runner.run("required_by_policy", required_by_policy);
    runner.run("equality_same", equality_same);
    runner.run("equality_different", equality_different);
    std::cout << "\n";
}
