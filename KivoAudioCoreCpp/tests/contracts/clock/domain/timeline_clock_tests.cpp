// =============================================================================
// KivoAudioCoreCpp — timeline_clock_tests.cpp
// Tests for TimelineClock contract type
// =============================================================================

#include "kivo/core/contract/clock/domain/timeline_clock.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// Tests
// =============================================================================
static void default_construction_is_invalid() {
    TimelineClock clock;
    ASSERT(!clock.is_valid());
    ASSERT(clock.position == kInvalidSamplePosition);
    ASSERT(clock.generation == GenerationId::initial());
    ASSERT(!clock.is_playing);
}

static void valid_construction() {
    TimelineClock clock;
    clock.position = 44100;
    clock.generation = GenerationId::next(GenerationId::initial());
    clock.is_playing = true;
    ASSERT(clock.is_valid());
}

static void invalid_position() {
    TimelineClock clock;
    clock.position = kInvalidSamplePosition;
    clock.is_playing = true;
    ASSERT(!clock.is_valid());
}

static void valid_not_playing() {
    TimelineClock clock;
    clock.position = 100;
    clock.is_playing = false;
    ASSERT(clock.is_valid());
}

static void equality_same() {
    TimelineClock a;
    a.position = 44100;
    a.generation = GenerationId::initial();
    a.is_playing = true;

    TimelineClock b = a;
    ASSERT(a == b);
}

static void equality_different_position() {
    TimelineClock a;
    a.position = 100;
    a.is_playing = true;

    TimelineClock b;
    b.position = 200;
    b.is_playing = true;

    ASSERT(!(a == b));
}

static void equality_different_playing() {
    TimelineClock a;
    a.position = 100;
    a.is_playing = true;

    TimelineClock b;
    b.position = 100;
    b.is_playing = false;

    ASSERT(!(a == b));
}

// =============================================================================
// Runner
// =============================================================================
void run_timeline_clock_tests(ContractTestRunner& runner) {
    std::cout << "--- TimelineClock ---\n";
    runner.run("default_construction_is_invalid", default_construction_is_invalid);
    runner.run("valid_construction", valid_construction);
    runner.run("invalid_position", invalid_position);
    runner.run("valid_not_playing", valid_not_playing);
    runner.run("equality_same", equality_same);
    runner.run("equality_different_position", equality_different_position);
    runner.run("equality_different_playing", equality_different_playing);
    std::cout << "\n";
}
