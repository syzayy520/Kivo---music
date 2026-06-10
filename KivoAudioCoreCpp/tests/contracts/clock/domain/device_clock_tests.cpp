// =============================================================================
// KivoAudioCoreCpp — device_clock_tests.cpp
// Tests for DeviceClock contract type
// =============================================================================

#include "kivo/core/contract/clock/device_clock.hpp"
#include "../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// Tests
// =============================================================================
static void default_construction_is_invalid() {
    DeviceClock clock;
    ASSERT(!clock.is_valid());
    ASSERT(clock.position == kInvalidSamplePosition);
    ASSERT(clock.generation == GenerationId::initial());
    ASSERT(clock.timestamp_qpc == 0);
}

static void valid_construction() {
    DeviceClock clock;
    clock.position = 44100;
    clock.generation = GenerationId::next(GenerationId::initial());
    clock.timestamp_qpc = 123456789;
    ASSERT(clock.is_valid());
}

static void invalid_position() {
    DeviceClock clock;
    clock.position = kInvalidSamplePosition;
    clock.timestamp_qpc = 100;
    ASSERT(!clock.is_valid());
}

static void invalid_timestamp() {
    DeviceClock clock;
    clock.position = 100;
    clock.timestamp_qpc = 0;
    ASSERT(!clock.is_valid());
}

static void equality_same() {
    DeviceClock a;
    a.position = 44100;
    a.generation = GenerationId::initial();
    a.timestamp_qpc = 100;

    DeviceClock b = a;
    ASSERT(a == b);
}

static void equality_different_position() {
    DeviceClock a;
    a.position = 100;
    a.timestamp_qpc = 100;

    DeviceClock b;
    b.position = 200;
    b.timestamp_qpc = 100;

    ASSERT(!(a == b));
}

static void equality_different_timestamp() {
    DeviceClock a;
    a.position = 100;
    a.timestamp_qpc = 100;

    DeviceClock b;
    b.position = 100;
    b.timestamp_qpc = 200;

    ASSERT(!(a == b));
}

// =============================================================================
// Runner
// =============================================================================
void run_device_clock_tests(ContractTestRunner& runner) {
    std::cout << "--- DeviceClock ---\n";
    runner.run("default_construction_is_invalid", default_construction_is_invalid);
    runner.run("valid_construction", valid_construction);
    runner.run("invalid_position", invalid_position);
    runner.run("invalid_timestamp", invalid_timestamp);
    runner.run("equality_same", equality_same);
    runner.run("equality_different_position", equality_different_position);
    runner.run("equality_different_timestamp", equality_different_timestamp);
    std::cout << "\n";
}
