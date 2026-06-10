// =============================================================================
// KivoAudioCoreCpp — seek_flush_tests.cpp
// Tests for SeekAccuracy/FlushScope/DrainSemantics contract types
// =============================================================================

#include "kivo/core/contract/seek_flush.hpp"
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
// SeekAccuracy tests
// =============================================================================
void seek_accuracy_values() {
    ASSERT(SeekAccuracy::Exact != SeekAccuracy::NearestKeyframe);
    ASSERT(SeekAccuracy::NearestKeyframe != SeekAccuracy::NearestSample);
}

// =============================================================================
// FlushScope tests
// =============================================================================
void flush_scope_none_is_zero() {
    ASSERT(static_cast<uint8_t>(FlushScope::None) == 0);
}

void flush_scope_all_is_or() {
    auto all = FlushScope::Decoder | FlushScope::Queue | FlushScope::Renderer;
    ASSERT(all == FlushScope::All);
}

void flush_scope_bitwise_or() {
    auto result = FlushScope::Decoder | FlushScope::Queue;
    ASSERT(has_flag(result, FlushScope::Decoder));
    ASSERT(has_flag(result, FlushScope::Queue));
    ASSERT(!has_flag(result, FlushScope::Renderer));
}

void flush_scope_has_flag() {
    ASSERT(has_flag(FlushScope::All, FlushScope::Decoder));
    ASSERT(has_flag(FlushScope::All, FlushScope::Queue));
    ASSERT(has_flag(FlushScope::All, FlushScope::Renderer));
    ASSERT(!has_flag(FlushScope::None, FlushScope::Decoder));
}

void flush_scope_bitwise_and() {
    auto result = FlushScope::All & FlushScope::Decoder;
    ASSERT(result == FlushScope::Decoder);
}

// =============================================================================
// DrainSemantics tests
// =============================================================================
void drain_semantics_values() {
    ASSERT(DrainSemantics::None != DrainSemantics::DrainToZero);
    ASSERT(DrainSemantics::DrainToZero != DrainSemantics::DrainToSilence);
    ASSERT(DrainSemantics::DrainToSilence != DrainSemantics::Immediate);
}

// =============================================================================
// Main
// =============================================================================
int main() {
    std::cout << "seek_flush_tests:\n";

    TEST(seek_accuracy_values);
    TEST(flush_scope_none_is_zero);
    TEST(flush_scope_all_is_or);
    TEST(flush_scope_bitwise_or);
    TEST(flush_scope_has_flag);
    TEST(flush_scope_bitwise_and);
    TEST(drain_semantics_values);

    std::cout << "\n  " << tests_passed << "/" << tests_run << " passed\n";
    return (tests_passed == tests_run) ? 0 : 1;
}
