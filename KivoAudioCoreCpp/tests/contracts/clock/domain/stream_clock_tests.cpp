// =============================================================================
// KivoAudioCoreCpp — stream_clock_tests.cpp
// Tests for StreamClock contract type
// =============================================================================

#include "kivo/core/contract/clock/domain/stream_clock.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// Tests
// =============================================================================
static void default_construction_is_invalid() {
    StreamClock clock;
    ASSERT(!clock.is_valid());
    ASSERT(clock.position == kInvalidSamplePosition);
    ASSERT(clock.generation == GenerationId::initial());
    ASSERT(clock.sample_rate == 0);
}

static void valid_construction() {
    StreamClock clock;
    clock.position = 44100;
    clock.generation = GenerationId::next(GenerationId::initial());
    clock.sample_rate = 44100;
    ASSERT(clock.is_valid());
}

static void invalid_position() {
    StreamClock clock;
    clock.position = kInvalidSamplePosition;
    clock.sample_rate = 44100;
    ASSERT(!clock.is_valid());
}

static void invalid_sample_rate() {
    StreamClock clock;
    clock.position = 100;
    clock.sample_rate = 0;
    ASSERT(!clock.is_valid());
}

static void equality_same() {
    StreamClock a;
    a.position = 44100;
    a.generation = GenerationId::initial();
    a.sample_rate = 44100;

    StreamClock b = a;
    ASSERT(a == b);
}

static void equality_different_position() {
    StreamClock a;
    a.position = 100;
    a.sample_rate = 44100;

    StreamClock b;
    b.position = 200;
    b.sample_rate = 44100;

    ASSERT(!(a == b));
}

static void equality_different_sample_rate() {
    StreamClock a;
    a.position = 100;
    a.sample_rate = 44100;

    StreamClock b;
    b.position = 100;
    b.sample_rate = 48000;

    ASSERT(!(a == b));
}

// =============================================================================
// Runner
// =============================================================================
void run_stream_clock_tests(ContractTestRunner& runner) {
    std::cout << "--- StreamClock ---\n";
    runner.run("default_construction_is_invalid", default_construction_is_invalid);
    runner.run("valid_construction", valid_construction);
    runner.run("invalid_position", invalid_position);
    runner.run("invalid_sample_rate", invalid_sample_rate);
    runner.run("equality_same", equality_same);
    runner.run("equality_different_position", equality_different_position);
    runner.run("equality_different_sample_rate", equality_different_sample_rate);
    std::cout << "\n";
}
