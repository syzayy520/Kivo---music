// =============================================================================
// KivoAudioCoreCpp — clock_domain_tests.cpp
// Tests for ClockDomain/ClockSnapshot contract types
// =============================================================================

#include "kivo/core/contract/clock_domain.hpp"
#include <cassert>
#include <iostream>

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
// ClockDomain enum tests
// =============================================================================
void clock_domain_values() {
    ASSERT(static_cast<uint8_t>(ClockDomain::Unknown) == 0);
    ASSERT(ClockDomain::Device != ClockDomain::Stream);
    ASSERT(ClockDomain::Stream != ClockDomain::Timeline);
}

// =============================================================================
// ClockSnapshot tests
// =============================================================================
void default_snapshot_is_invalid() {
    ClockSnapshot snap;
    ASSERT(!snap.is_valid());
}

void valid_snapshot() {
    ClockSnapshot snap;
    snap.domain = ClockDomain::Stream;
    snap.position = 44100;
    snap.generation = GenerationId::initial();
    ASSERT(snap.is_valid());
}

void invalid_unknown_domain() {
    ClockSnapshot snap;
    snap.domain = ClockDomain::Unknown;
    snap.position = 100;
    ASSERT(!snap.is_valid());
}

void invalid_position() {
    ClockSnapshot snap;
    snap.domain = ClockDomain::Device;
    snap.position = kInvalidSamplePosition;
    ASSERT(!snap.is_valid());
}

void equality_same() {
    ClockSnapshot a;
    a.domain = ClockDomain::Timeline;
    a.position = 48000;
    a.generation = GenerationId::next(GenerationId::initial());

    ClockSnapshot b = a;
    ASSERT(a == b);
}

void equality_different() {
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
// Main
// =============================================================================
int main() {
    std::cout << "clock_domain_tests:\n";

    TEST(clock_domain_values);
    TEST(default_snapshot_is_invalid);
    TEST(valid_snapshot);
    TEST(invalid_unknown_domain);
    TEST(invalid_position);
    TEST(equality_same);
    TEST(equality_different);

    std::cout << "\n  " << tests_passed << "/" << tests_run << " passed\n";
    return (tests_passed == tests_run) ? 0 : 1;
}
