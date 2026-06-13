#include <iostream>
#include <iterator>

#include "output_truth_test_cases.hpp"

int main() {
    using namespace output_truth_test;
    struct Test {
        const char* name;
        void (*run)();
    };
    const Test tests[] = {
        {"complete_clean_evidence_is_bit_perfect",
         complete_clean_evidence_is_bit_perfect},
        {"incomplete_evidence_is_rejected",
         incomplete_evidence_is_rejected},
        {"policy_forbidden_is_rejected",
         policy_forbidden_is_rejected},
        {"shared_and_mismatched_modes_are_rejected",
         shared_and_mismatched_modes_are_rejected},
        {"format_mismatches_are_classified",
         format_mismatches_are_classified},
        {"engine_resample_and_conversion_are_rejected",
         engine_resample_and_conversion_are_rejected},
        {"processing_and_dither_are_rejected",
         processing_and_dither_are_rejected},
        {"snapshots_build_complete_truth_evidence",
         snapshots_build_complete_truth_evidence}
    };
    int passed = 0;
    std::cout << "=== Output Truth Tests ===\n\n";
    for (const auto& test : tests) {
        std::cout << "  " << test.name << "... ";
        try {
            test.run();
            ++passed;
            std::cout << "PASS\n";
        } catch (const std::exception& exception) {
            std::cout << "FAIL: " << exception.what() << "\n";
        }
    }
    std::cout << "\n=== " << passed << "/" << std::size(tests)
              << " passed ===\n";
    return passed == std::size(tests) ? 0 : 1;
}
