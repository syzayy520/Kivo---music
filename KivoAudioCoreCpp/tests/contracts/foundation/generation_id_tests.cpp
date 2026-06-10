// =============================================================================
// KivoAudioCoreCpp — generation_id_tests.cpp
// Tests for GenerationId contract type
// =============================================================================

#include "kivo/core/contract/generation_id.hpp"
#include "../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// Tests
// =============================================================================
static void initial_value_is_zero() {
    auto g = GenerationId::initial();
    ASSERT(g.value() == 0);
}

static void next_increments() {
    auto g0 = GenerationId::initial();
    auto g1 = GenerationId::next(g0);
    ASSERT(g1.value() == 1);
}

static void next_chain() {
    auto g = GenerationId::initial();
    for (uint64_t i = 0; i < 100; ++i) {
        g = GenerationId::next(g);
    }
    ASSERT(g.value() == 100);
}

static void equality_reflexive() {
    auto g = GenerationId::initial();
    ASSERT(g == g);
}

static void equality_same_value() {
    auto a = GenerationId::next(GenerationId::initial());
    auto b = GenerationId::next(GenerationId::initial());
    ASSERT(a == b);
}

static void inequality_different_value() {
    auto a = GenerationId::initial();
    auto b = GenerationId::next(a);
    ASSERT(!(a == b));
}

static void ordering() {
    auto a = GenerationId::initial();
    auto b = GenerationId::next(a);
    ASSERT(a < b);
    ASSERT(b > a);
    ASSERT(a <= b);
    ASSERT(b >= a);
}

// =============================================================================
// Runner
// =============================================================================
void run_generation_id_contract_tests(ContractTestRunner& runner) {
    std::cout << "--- GenerationId ---\n";
    runner.run("initial_value_is_zero", initial_value_is_zero);
    runner.run("next_increments", next_increments);
    runner.run("next_chain", next_chain);
    runner.run("equality_reflexive", equality_reflexive);
    runner.run("equality_same_value", equality_same_value);
    runner.run("inequality_different_value", inequality_different_value);
    runner.run("ordering", ordering);
    std::cout << "\n";
}
