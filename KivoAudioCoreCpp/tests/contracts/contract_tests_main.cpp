// =============================================================================
// KivoAudioCoreCpp — contract_tests_main.cpp
// Contract Test Runner — Single entry point for all contract tests
// =============================================================================
//
// This file is the ONLY file with main().
// Each family test file exports a run_*_tests(ContractTestRunner&) function.
// No test logic is duplicated here.
// =============================================================================

#include "contract_tests_main.h"

// Family test runner declarations
void run_result_contract_tests(ContractTestRunner& runner);
void run_generation_id_contract_tests(ContractTestRunner& runner);
void run_sample_position_contract_tests(ContractTestRunner& runner);
void run_audio_format_descriptor_contract_tests(ContractTestRunner& runner);
void run_clock_domain_contract_tests(ContractTestRunner& runner);
void run_seek_flush_contract_tests(ContractTestRunner& runner);
void run_device_clock_tests(ContractTestRunner& runner);
void run_stream_clock_tests(ContractTestRunner& runner);
void run_timeline_clock_tests(ContractTestRunner& runner);
void run_position_tests(ContractTestRunner& runner);
void run_drift_estimate_tests(ContractTestRunner& runner);
void run_clock_policy_tests(ContractTestRunner& runner);

// =============================================================================
// Main
// =============================================================================
int main() {
    std::cout << "=== Contract Tests ===\n\n";

    ContractTestRunner runner;

    run_result_contract_tests(runner);
    run_generation_id_contract_tests(runner);
    run_sample_position_contract_tests(runner);
    run_audio_format_descriptor_contract_tests(runner);
    run_clock_domain_contract_tests(runner);
    run_seek_flush_contract_tests(runner);
    run_device_clock_tests(runner);
    run_stream_clock_tests(runner);
    run_timeline_clock_tests(runner);
    run_position_tests(runner);
    run_drift_estimate_tests(runner);
    run_clock_policy_tests(runner);

    std::cout << "=== " << runner.tests_passed << "/" << runner.tests_run << " passed ===\n";
    return runner.exit_code();
}
