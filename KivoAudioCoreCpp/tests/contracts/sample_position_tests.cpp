// =============================================================================
// KivoAudioCoreCpp — sample_position_tests.cpp
// Tests for SamplePosition/FrameCount type aliases
// =============================================================================

#include "kivo/core/contract/sample_position.hpp"
#include <cassert>
#include <iostream>
#include <type_traits>

using namespace kivo::core::contract;

// =============================================================================
// Test helpers
// =============================================================================
static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) \
    do { \
        tests_run++; \
        std::cout << "  " << #name << "... "; \
        try { \
            name(); \
            tests_passed++; \
            std::cout << "PASS\n"; \
        } catch (const std::exception& e) { \
            std::cout << "FAIL: " << e.what() << "\n"; \
        } \
    } while(0)

#define ASSERT(cond) \
    do { \
        if (!(cond)) { \
            throw std::runtime_error("Assertion failed: " #cond); \
        } \
    } while(0)

// =============================================================================
// Tests
// =============================================================================
void sample_position_is_uint64() {
    static_assert(std::is_same_v<SamplePosition, uint64_t>);
    ASSERT(true);
}

void frame_count_is_uint64() {
    static_assert(std::is_same_v<FrameCount, uint64_t>);
    ASSERT(true);
}

void invalid_position_is_max() {
    ASSERT(kInvalidSamplePosition == UINT64_MAX);
}

void zero_frame_count_is_zero() {
    ASSERT(kZeroFrameCount == 0);
}

void sample_position_arithmetic() {
    SamplePosition pos = 100;
    pos += 50;
    ASSERT(pos == 150);
}

void frame_count_arithmetic() {
    FrameCount count = 1024;
    count -= 256;
    ASSERT(count == 768);
}

// =============================================================================
// Main
// =============================================================================
int main() {
    std::cout << "sample_position_tests:\n";

    TEST(sample_position_is_uint64);
    TEST(frame_count_is_uint64);
    TEST(invalid_position_is_max);
    TEST(zero_frame_count_is_zero);
    TEST(sample_position_arithmetic);
    TEST(frame_count_arithmetic);

    std::cout << "\n  " << tests_passed << "/" << tests_run << " passed\n";
    return (tests_passed == tests_run) ? 0 : 1;
}
