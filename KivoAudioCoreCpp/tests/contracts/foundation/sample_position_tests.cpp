// =============================================================================
// KivoAudioCoreCpp — sample_position_tests.cpp
// Tests for SamplePosition/FrameCount type aliases
// =============================================================================

#include "kivo/core/contract/sample_position.hpp"
#include "../contract_tests_main.h"
#include <type_traits>

using namespace kivo::core::contract;

// =============================================================================
// Tests
// =============================================================================
static void sample_position_is_uint64() {
    static_assert(std::is_same_v<SamplePosition, uint64_t>);
    ASSERT(true);
}

static void frame_count_is_uint64() {
    static_assert(std::is_same_v<FrameCount, uint64_t>);
    ASSERT(true);
}

static void invalid_position_is_max() {
    ASSERT(kInvalidSamplePosition == UINT64_MAX);
}

static void zero_frame_count_is_zero() {
    ASSERT(kZeroFrameCount == 0);
}

static void sample_position_arithmetic() {
    SamplePosition pos = 100;
    pos += 50;
    ASSERT(pos == 150);
}

static void frame_count_arithmetic() {
    FrameCount count = 1024;
    count -= 256;
    ASSERT(count == 768);
}

// =============================================================================
// Runner
// =============================================================================
void run_sample_position_contract_tests(ContractTestRunner& runner) {
    std::cout << "--- SamplePosition/FrameCount ---\n";
    runner.run("sample_position_is_uint64", sample_position_is_uint64);
    runner.run("frame_count_is_uint64", frame_count_is_uint64);
    runner.run("invalid_position_is_max", invalid_position_is_max);
    runner.run("zero_frame_count_is_zero", zero_frame_count_is_zero);
    runner.run("sample_position_arithmetic", sample_position_arithmetic);
    runner.run("frame_count_arithmetic", frame_count_arithmetic);
    std::cout << "\n";
}
