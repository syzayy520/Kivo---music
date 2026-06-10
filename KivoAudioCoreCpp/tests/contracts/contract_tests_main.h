// =============================================================================
// KivoAudioCoreCpp — contract_tests_main.h
// Contract Test Infrastructure — Shared runner and macros
// =============================================================================
//
// Provides ContractTestRunner, TEST, and ASSERT macros for all contract tests.
// Only contract_tests_main.cpp includes this as the entry point.
// Family test files include this to use runner.run() and ASSERT.
// =============================================================================

#pragma once

#include <iostream>
#include <stdexcept>
#include <string>

// =============================================================================
// ContractTestRunner — Lightweight test runner
// =============================================================================
struct ContractTestRunner {
    int tests_run = 0;
    int tests_passed = 0;

    void run(const char* name, void(*test_fn)()) {
        tests_run++;
        std::cout << "  " << name << "... ";
        try {
            test_fn();
            tests_passed++;
            std::cout << "PASS\n";
        } catch (const std::exception& e) {
            std::cout << "FAIL: " << e.what() << "\n";
        }
    }

    [[nodiscard]] int exit_code() const {
        return (tests_passed == tests_run) ? 0 : 1;
    }
};

// =============================================================================
// TEST / ASSERT macros
// =============================================================================
#define ASSERT(cond) \
    do { \
        if (!(cond)) { \
            throw std::runtime_error("Assertion failed: " #cond); \
        } \
    } while(0)
