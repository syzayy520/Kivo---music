#pragma once

#include <iostream>
#include <stdexcept>

struct DecodeBoundaryTestRunner {
    int tests_run{0};
    int tests_passed{0};

    void run(const char* name, void (*test_fn)()) {
        ++tests_run;
        std::cout << "  " << name << "... ";
        try {
            test_fn();
            ++tests_passed;
            std::cout << "PASS\n";
        } catch (const std::exception& exception) {
            std::cout << "FAIL: " << exception.what() << "\n";
        }
    }

    [[nodiscard]] int exit_code() const noexcept {
        return tests_run == tests_passed ? 0 : 1;
    }
};

#define DECODE_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            throw std::runtime_error("Assertion failed: " #condition); \
        } \
    } while (false)
