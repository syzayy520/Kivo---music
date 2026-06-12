#include <iostream>
#include <iterator>
#include <stdexcept>

#include "hires_pcm_test_cases.hpp"

namespace {

#define ASSERT(cond) \
    do { \
        if (!(cond)) { \
            throw std::runtime_error("Assertion failed: " #cond); \
        } \
    } while (0)

} // namespace

int main() {
    using namespace hires_pcm_test;
    struct Test {
        const char* name;
        void (*run)();
    };
    const Test tests[] = {
        {"high_sample_rate_pcm_descriptors_are_valid",
         high_sample_rate_pcm_descriptors_are_valid},
        {"valid_bits_and_container_bits_remain_distinct",
         valid_bits_and_container_bits_remain_distinct},
        {"frame_size_uses_container_bits",
         frame_size_uses_container_bits},
        {"pcm_sample_formats_remain_integer_or_float",
         pcm_sample_formats_remain_integer_or_float},
        {"unsupported_non_pcm_payload_is_not_modelled_as_pcm",
         unsupported_non_pcm_payload_is_not_modelled_as_pcm}
    };
    int passed = 0;
    std::cout << "=== Hi-Res PCM Tests ===\n\n";
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
