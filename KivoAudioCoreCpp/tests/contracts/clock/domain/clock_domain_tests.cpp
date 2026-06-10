// =============================================================================
// KivoAudioCoreCpp — clock_domain_tests.cpp
// Tests for ClockDomain/ClockSnapshot contract types
// =============================================================================

#include "kivo/core/contract/clock/domain/clock_domain.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// Tests
// =============================================================================
static void clock_domain_values() {
    ASSERT(static_cast<uint8_t>(ClockDomain::Unknown) == 0);
    ASSERT(ClockDomain::Device != ClockDomain::Stream);
    ASSERT(ClockDomain::Stream != ClockDomain::Timeline);
}

static void default_snapshot_is_invalid() {
    ClockSnapshot snap;
    ASSERT(!snap.is_valid());
}

static void valid_snapshot() {
    ClockSnapshot snap;
    snap.domain = ClockDomain::Stream;
    snap.position = 44100;
    snap.generation = GenerationId::initial();
    ASSERT(snap.is_valid());
}

static void invalid_unknown_domain() {
    ClockSnapshot snap;
    snap.domain = ClockDomain::Unknown;
    snap.position = 100;
    ASSERT(!snap.is_valid());
}

static void invalid_position() {
    ClockSnapshot snap;
    snap.domain = ClockDomain::Device;
    snap.position = kInvalidSamplePosition;
    ASSERT(!snap.is_valid());
}

static void equality_same() {
    ClockSnapshot a;
    a.domain = ClockDomain::Timeline;
    a.position = 48000;
    a.generation = GenerationId::next(GenerationId::initial());

    ClockSnapshot b = a;
    ASSERT(a == b);
}

static void equality_different() {
    ClockSnapshot a;
    a.domain = ClockDomain::Stream;
    a.position = 100;
    a.generation = GenerationId::initial();

    ClockSnapshot b;
    b.domain = ClockDomain::Stream;
    b.position = 200;
    b.generation = GenerationId::initial();

    ASSERT(!(a == b));
}

// =============================================================================
// Runner
// =============================================================================
void run_clock_domain_contract_tests(ContractTestRunner& runner) {
    std::cout << "--- ClockDomain/ClockSnapshot ---\n";
    runner.run("clock_domain_values", clock_domain_values);
    runner.run("default_snapshot_is_invalid", default_snapshot_is_invalid);
    runner.run("valid_snapshot", valid_snapshot);
    runner.run("invalid_unknown_domain", invalid_unknown_domain);
    runner.run("invalid_position", invalid_position);
    runner.run("equality_same", equality_same);
    runner.run("equality_different", equality_different);
    std::cout << "\n";
}
