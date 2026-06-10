// =============================================================================
// KivoAudioCoreCpp — seek_target_tests.cpp
// Tests for SeekTarget contract type
// =============================================================================

#include "kivo/core/contract/seek/seek_target.hpp"
#include "../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// Tests
// =============================================================================
static void seek_target_default_construction_is_invalid() {
    SeekTarget target;
    ASSERT(!target.is_valid());
    ASSERT(target.position == kInvalidSamplePosition);
    ASSERT(target.generation == GenerationId::initial());
    ASSERT(target.accuracy == SeekAccuracy::Exact);
}

static void seek_target_valid_construction() {
    auto gen = GenerationId::next(GenerationId::initial()); // gen 1
    SeekTarget target{1000, gen, SeekAccuracy::NearestKeyframe};
    ASSERT(target.is_valid());
    ASSERT(target.position == 1000);
    ASSERT(target.generation == gen);
    ASSERT(target.accuracy == SeekAccuracy::NearestKeyframe);
}

static void seek_target_invalid_position() {
    auto gen = GenerationId::next(GenerationId::initial());
    SeekTarget target{kInvalidSamplePosition, gen, SeekAccuracy::Exact};
    ASSERT(!target.is_valid());
}

static void seek_target_equality_same() {
    auto gen = GenerationId::next(GenerationId::initial());
    SeekTarget a{1000, gen, SeekAccuracy::Exact};
    SeekTarget b{1000, gen, SeekAccuracy::Exact};
    ASSERT(a == b);
}

static void seek_target_equality_different_position() {
    auto gen = GenerationId::next(GenerationId::initial());
    SeekTarget a{1000, gen, SeekAccuracy::Exact};
    SeekTarget b{2000, gen, SeekAccuracy::Exact};
    ASSERT(!(a == b));
}

static void seek_target_equality_different_generation() {
    auto gen1 = GenerationId::next(GenerationId::initial());
    auto gen2 = GenerationId::next(gen1);
    SeekTarget a{1000, gen1, SeekAccuracy::Exact};
    SeekTarget b{1000, gen2, SeekAccuracy::Exact};
    ASSERT(!(a == b));
}

static void seek_target_equality_different_accuracy() {
    auto gen = GenerationId::next(GenerationId::initial());
    SeekTarget a{1000, gen, SeekAccuracy::Exact};
    SeekTarget b{1000, gen, SeekAccuracy::NearestKeyframe};
    ASSERT(!(a == b));
}

// =============================================================================
// Runner
// =============================================================================
void run_seek_target_tests(ContractTestRunner& runner) {
    std::cout << "--- SeekTarget ---\n";
    runner.run("seek_target_default_construction_is_invalid", seek_target_default_construction_is_invalid);
    runner.run("seek_target_valid_construction", seek_target_valid_construction);
    runner.run("seek_target_invalid_position", seek_target_invalid_position);
    runner.run("seek_target_equality_same", seek_target_equality_same);
    runner.run("seek_target_equality_different_position", seek_target_equality_different_position);
    runner.run("seek_target_equality_different_generation", seek_target_equality_different_generation);
    runner.run("seek_target_equality_different_accuracy", seek_target_equality_different_accuracy);
    std::cout << "\n";
}
