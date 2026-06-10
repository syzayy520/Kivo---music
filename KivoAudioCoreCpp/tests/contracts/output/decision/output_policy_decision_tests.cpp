// =============================================================================
// KivoAudioCoreCpp — output_policy_decision_tests.cpp
// Tests for OutputPolicyDecision contract type
// =============================================================================

#include "kivo/core/contract/output/decision/output_policy_decision.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void default_decision_is_invalid() {
    OutputPolicyDecision decision;
    ASSERT(!decision.is_valid());
    ASSERT(decision.mode == OutputMode::Unknown);
}

static void shared_mode_is_valid() {
    OutputPolicyDecision decision;
    decision.mode = OutputMode::Shared;
    ASSERT(decision.is_valid());
    ASSERT(!decision.is_bit_perfect());
}

static void bit_perfect_active_mode() {
    OutputPolicyDecision decision;
    decision.mode = OutputMode::BitPerfectActive;
    decision.bit_perfect_active = true;
    ASSERT(decision.is_valid());
    ASSERT(decision.is_bit_perfect());
}

static void explanation_preserved() {
    OutputPolicyDecision decision;
    decision.explanation.format_conversion_performed = true;
    ASSERT(decision.explanation.format_conversion_performed);
}

static void equality_same() {
    OutputPolicyDecision a;
    a.mode = OutputMode::Shared;
    OutputPolicyDecision b = a;
    ASSERT(a == b);
}

static void equality_different() {
    OutputPolicyDecision a;
    a.mode = OutputMode::Shared;
    OutputPolicyDecision b;
    b.mode = OutputMode::Exclusive;
    ASSERT(!(a == b));
}

void run_output_policy_decision_tests(ContractTestRunner& runner) {
    std::cout << "--- OutputPolicyDecision ---\n";
    runner.run("default_decision_is_invalid", default_decision_is_invalid);
    runner.run("shared_mode_is_valid", shared_mode_is_valid);
    runner.run("bit_perfect_active_mode", bit_perfect_active_mode);
    runner.run("explanation_preserved", explanation_preserved);
    runner.run("equality_same", equality_same);
    runner.run("equality_different", equality_different);
    std::cout << "\n";
}
