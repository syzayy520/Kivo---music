// =============================================================================
// KivoAudioCoreCpp — capability_tests_main.cpp
// Capability Contract Tests — Single entry point for all capability tests
// =============================================================================
//
// This file is the ONLY file with main() for kivo_capability_tests.
// Each family test file exports a run_*_tests(ContractTestRunner&) function.
// =============================================================================

#include "../contract_tests_main.h"

// Family test runner declarations
void run_identity_contract_tests(ContractTestRunner& runner);
void run_quality_contract_tests(ContractTestRunner& runner);
void run_constraint_contract_tests(ContractTestRunner& runner);
void run_component_contract_tests(ContractTestRunner& runner);
void run_domain_contract_tests(ContractTestRunner& runner);
void run_negotiation_contract_tests(ContractTestRunner& runner);

// =============================================================================
// Main
// =============================================================================
int main() {
    std::cout << "=== Capability Contract Tests ===\n\n";

    ContractTestRunner runner;

    run_identity_contract_tests(runner);
    run_quality_contract_tests(runner);
    run_constraint_contract_tests(runner);
    run_component_contract_tests(runner);
    run_domain_contract_tests(runner);
    run_negotiation_contract_tests(runner);

    std::cout << "=== " << runner.tests_passed << "/" << runner.tests_run << " passed ===\n";
    return runner.exit_code();
}
