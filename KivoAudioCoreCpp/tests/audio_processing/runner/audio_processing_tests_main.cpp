#include <iostream>
#include <iterator>

#include "audio_processing_test_cases.hpp"

int main() {
    using namespace audio_processing_test;
    struct Test {
        const char* name;
        void (*run)();
    };
    const Test tests[] = {
        {"default_bypass_is_byte_identical",
         default_bypass_is_byte_identical},
        {"unity_enabled_stages_are_bypassed",
         unity_enabled_stages_are_bypassed},
        {"replay_gain_and_volume_are_independent",
         replay_gain_and_volume_are_independent},
        {"peak_prevention_limits_gain",
         peak_prevention_limits_gain},
        {"integer_formats_apply_gain_and_saturate",
         integer_formats_apply_gain_and_saturate},
        {"floating_formats_apply_gain_and_saturate",
         floating_formats_apply_gain_and_saturate},
        {"triangular_dither_is_deterministic",
         triangular_dither_is_deterministic},
        {"bit_perfect_conflicts_are_rejected",
         bit_perfect_conflicts_are_rejected},
        {"invalid_policy_is_rejected",
         invalid_policy_is_rejected}
    };
    int passed = 0;
    std::cout << "=== Audio Processing Tests ===\n\n";
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
