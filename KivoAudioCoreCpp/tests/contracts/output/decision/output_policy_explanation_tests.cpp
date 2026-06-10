// =============================================================================
// KivoAudioCoreCpp — output_policy_explanation_tests.cpp
// Tests for OutputPolicyExplanation contract type
// =============================================================================

#include "kivo/core/contract/output/decision/output_policy_explanation.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void default_has_no_blocked() {
    OutputPolicyExplanation expl;
    ASSERT(!expl.bit_perfect_blocked());
}

static void bit_perfect_blocked() {
    OutputPolicyExplanation expl;
    expl.bit_perfect_applicable = true;
    expl.bit_perfect_achieved = false;
    ASSERT(expl.bit_perfect_blocked());
}

static void bit_perfect_not_blocked_when_achieved() {
    OutputPolicyExplanation expl;
    expl.bit_perfect_applicable = true;
    expl.bit_perfect_achieved = true;
    ASSERT(!expl.bit_perfect_blocked());
}

static void equality_same() {
    OutputPolicyExplanation a;
    a.format_conversion_performed = true;
    OutputPolicyExplanation b = a;
    ASSERT(a == b);
}

static void equality_different() {
    OutputPolicyExplanation a;
    a.engine_in_path = true;
    OutputPolicyExplanation b;
    ASSERT(!(a == b));
}

void run_output_policy_explanation_tests(ContractTestRunner& runner) {
    std::cout << "--- OutputPolicyExplanation ---\n";
    runner.run("default_has_no_blocked", default_has_no_blocked);
    runner.run("bit_perfect_blocked", bit_perfect_blocked);
    runner.run("bit_perfect_not_blocked_when_achieved", bit_perfect_not_blocked_when_achieved);
    runner.run("equality_same", equality_same);
    runner.run("equality_different", equality_different);
    std::cout << "\n";
}
