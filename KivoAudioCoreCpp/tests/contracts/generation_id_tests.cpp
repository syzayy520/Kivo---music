// =============================================================================
// KivoAudioCoreCpp — generation_id_tests.cpp
// Tests for GenerationId contract type
// =============================================================================

#include "kivo/core/contract/generation_id.hpp"
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
// Tests
// =============================================================================
void initial_value_is_zero() {
    auto g = GenerationId::initial();
    ASSERT(g.value() == 0);
}

void next_increments() {
    auto g0 = GenerationId::initial();
    auto g1 = GenerationId::next(g0);
    ASSERT(g1.value() == 1);
}

void next_chain() {
    auto g = GenerationId::initial();
    for (uint64_t i = 0; i < 100; ++i) {
        g = GenerationId::next(g);
    }
    ASSERT(g.value() == 100);
}

void equality_reflexive() {
    auto g = GenerationId::initial();
    ASSERT(g == g);
}

void equality_same_value() {
    auto a = GenerationId::next(GenerationId::initial());
    auto b = GenerationId::next(GenerationId::initial());
    ASSERT(a == b);
}

void inequality_different_value() {
    auto a = GenerationId::initial();
    auto b = GenerationId::next(a);
    ASSERT(!(a == b));
}

void ordering() {
    auto a = GenerationId::initial();
    auto b = GenerationId::next(a);
    ASSERT(a < b);
    ASSERT(b > a);
    ASSERT(a <= b);
    ASSERT(b >= a);
}

// =============================================================================
// Main
// =============================================================================
int main() {
    std::cout << "generation_id_tests:\n";

    TEST(initial_value_is_zero);
    TEST(next_increments);
    TEST(next_chain);
    TEST(equality_reflexive);
    TEST(equality_same_value);
    TEST(inequality_different_value);
    TEST(ordering);

    std::cout << "\n  " << tests_passed << "/" << tests_run << " passed\n";
    return (tests_passed == tests_run) ? 0 : 1;
}
