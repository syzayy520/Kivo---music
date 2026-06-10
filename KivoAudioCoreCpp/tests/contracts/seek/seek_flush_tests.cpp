// =============================================================================
// KivoAudioCoreCpp — seek_flush_tests.cpp
// Tests for SeekAccuracy/FlushScope/DrainSemantics contract types
// =============================================================================

#include "kivo/core/contract/seek/seek_flush.hpp"
#include "../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// Tests
// =============================================================================
static void seek_accuracy_values() {
    ASSERT(SeekAccuracy::Exact != SeekAccuracy::NearestKeyframe);
    ASSERT(SeekAccuracy::NearestKeyframe != SeekAccuracy::NearestSample);
}

static void flush_scope_none_is_zero() {
    ASSERT(static_cast<uint8_t>(FlushScope::None) == 0);
}

static void flush_scope_all_is_or() {
    auto all = FlushScope::Decoder | FlushScope::Queue | FlushScope::Renderer;
    ASSERT(all == FlushScope::All);
}

static void flush_scope_bitwise_or() {
    auto result = FlushScope::Decoder | FlushScope::Queue;
    ASSERT(has_flag(result, FlushScope::Decoder));
    ASSERT(has_flag(result, FlushScope::Queue));
    ASSERT(!has_flag(result, FlushScope::Renderer));
}

static void flush_scope_has_flag() {
    ASSERT(has_flag(FlushScope::All, FlushScope::Decoder));
    ASSERT(has_flag(FlushScope::All, FlushScope::Queue));
    ASSERT(has_flag(FlushScope::All, FlushScope::Renderer));
    ASSERT(!has_flag(FlushScope::None, FlushScope::Decoder));
}

static void flush_scope_bitwise_and() {
    auto result = FlushScope::All & FlushScope::Decoder;
    ASSERT(result == FlushScope::Decoder);
}

static void drain_semantics_values() {
    ASSERT(DrainSemantics::None != DrainSemantics::DrainToZero);
    ASSERT(DrainSemantics::DrainToZero != DrainSemantics::DrainToSilence);
    ASSERT(DrainSemantics::DrainToSilence != DrainSemantics::Immediate);
}

// =============================================================================
// Runner
// =============================================================================
void run_seek_flush_contract_tests(ContractTestRunner& runner) {
    std::cout << "--- SeekAccuracy/FlushScope/DrainSemantics ---\n";
    runner.run("seek_accuracy_values", seek_accuracy_values);
    runner.run("flush_scope_none_is_zero", flush_scope_none_is_zero);
    runner.run("flush_scope_all_is_or", flush_scope_all_is_or);
    runner.run("flush_scope_bitwise_or", flush_scope_bitwise_or);
    runner.run("flush_scope_has_flag", flush_scope_has_flag);
    runner.run("flush_scope_bitwise_and", flush_scope_bitwise_and);
    runner.run("drain_semantics_values", drain_semantics_values);
    std::cout << "\n";
}
