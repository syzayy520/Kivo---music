// =============================================================================
// KivoAudioCoreCpp — seek_generation_tests.cpp
// Tests for SeekGeneration contract type
// =============================================================================

#include "kivo/core/contract/seek/generation/seek_generation.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// Tests
// =============================================================================
static void seek_generation_default_construction_is_valid() {
    // Default (no seeks) is valid
    SeekGeneration gen;
    ASSERT(gen.is_valid());
    ASSERT(gen.generation == GenerationId::initial());
    ASSERT(gen.seek_count == 0);
    ASSERT(gen.last_seek_qpc == 0);
}

static void seek_generation_valid_construction() {
    auto gen = GenerationId::next(GenerationId::initial());
    SeekGeneration sg{gen, 5, 123456789};
    ASSERT(sg.is_valid());
    ASSERT(sg.generation == gen);
    ASSERT(sg.seek_count == 5);
    ASSERT(sg.last_seek_qpc == 123456789);
}

static void seek_generation_invalid_seek_count_without_generation() {
    // If seek_count > 0, generation must not be initial
    SeekGeneration sg{GenerationId::initial(), 1, 123456789};
    ASSERT(!sg.is_valid());
}

static void seek_generation_valid_seek_count_with_generation() {
    auto gen = GenerationId::next(GenerationId::initial());
    SeekGeneration sg{gen, 1, 123456789};
    ASSERT(sg.is_valid());
}

static void seek_generation_equality_same() {
    auto gen = GenerationId::next(GenerationId::initial());
    SeekGeneration a{gen, 5, 123456789};
    SeekGeneration b{gen, 5, 123456789};
    ASSERT(a == b);
}

static void seek_generation_equality_different_generation() {
    auto gen1 = GenerationId::next(GenerationId::initial());
    auto gen2 = GenerationId::next(gen1);
    SeekGeneration a{gen1, 5, 123456789};
    SeekGeneration b{gen2, 5, 123456789};
    ASSERT(!(a == b));
}

static void seek_generation_equality_different_count() {
    auto gen = GenerationId::next(GenerationId::initial());
    SeekGeneration a{gen, 5, 123456789};
    SeekGeneration b{gen, 6, 123456789};
    ASSERT(!(a == b));
}

static void seek_generation_equality_different_timestamp() {
    auto gen = GenerationId::next(GenerationId::initial());
    SeekGeneration a{gen, 5, 123456789};
    SeekGeneration b{gen, 5, 987654321};
    ASSERT(!(a == b));
}

// =============================================================================
// Runner
// =============================================================================
void run_seek_generation_tests(ContractTestRunner& runner) {
    std::cout << "--- SeekGeneration ---\n";
    runner.run("seek_generation_default_construction_is_valid", seek_generation_default_construction_is_valid);
    runner.run("seek_generation_valid_construction", seek_generation_valid_construction);
    runner.run("seek_generation_invalid_seek_count_without_generation", seek_generation_invalid_seek_count_without_generation);
    runner.run("seek_generation_valid_seek_count_with_generation", seek_generation_valid_seek_count_with_generation);
    runner.run("seek_generation_equality_same", seek_generation_equality_same);
    runner.run("seek_generation_equality_different_generation", seek_generation_equality_different_generation);
    runner.run("seek_generation_equality_different_count", seek_generation_equality_different_count);
    runner.run("seek_generation_equality_different_timestamp", seek_generation_equality_different_timestamp);
    std::cout << "\n";
}
