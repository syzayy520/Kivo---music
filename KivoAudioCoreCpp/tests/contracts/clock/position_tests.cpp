// =============================================================================
// KivoAudioCoreCpp — position_tests.cpp
// Tests for DecodedPosition and RenderedPosition contract types
// =============================================================================

#include "kivo/core/contract/clock/decoded_position.hpp"
#include "kivo/core/contract/clock/rendered_position.hpp"
#include "../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// DecodedPosition Tests
// =============================================================================
static void decoded_default_construction_is_invalid() {
    DecodedPosition pos;
    ASSERT(!pos.is_valid());
    ASSERT(pos.sample == kInvalidSamplePosition);
    ASSERT(pos.frames_decoded == 0);
    ASSERT(pos.generation == GenerationId::initial());
}

static void decoded_valid_construction() {
    DecodedPosition pos;
    pos.sample = 44100;
    pos.frames_decoded = 1024;
    pos.generation = GenerationId::next(GenerationId::initial());
    ASSERT(pos.is_valid());
}

static void decoded_invalid_sample() {
    DecodedPosition pos;
    pos.sample = kInvalidSamplePosition;
    pos.frames_decoded = 1024;
    ASSERT(!pos.is_valid());
}

static void decoded_equality_same() {
    DecodedPosition a;
    a.sample = 44100;
    a.frames_decoded = 1024;
    a.generation = GenerationId::initial();

    DecodedPosition b = a;
    ASSERT(a == b);
}

static void decoded_equality_different() {
    DecodedPosition a;
    a.sample = 100;
    a.frames_decoded = 1024;

    DecodedPosition b;
    b.sample = 200;
    b.frames_decoded = 1024;

    ASSERT(!(a == b));
}

// =============================================================================
// RenderedPosition Tests
// =============================================================================
static void rendered_default_construction_is_invalid() {
    RenderedPosition pos;
    ASSERT(!pos.is_valid());
    ASSERT(pos.sample == kInvalidSamplePosition);
    ASSERT(pos.frames_written == 0);
    ASSERT(pos.generation == GenerationId::initial());
}

static void rendered_valid_construction() {
    RenderedPosition pos;
    pos.sample = 44100;
    pos.frames_written = 512;
    pos.generation = GenerationId::next(GenerationId::initial());
    ASSERT(pos.is_valid());
}

static void rendered_invalid_sample() {
    RenderedPosition pos;
    pos.sample = kInvalidSamplePosition;
    pos.frames_written = 512;
    ASSERT(!pos.is_valid());
}

static void rendered_equality_same() {
    RenderedPosition a;
    a.sample = 44100;
    a.frames_written = 512;
    a.generation = GenerationId::initial();

    RenderedPosition b = a;
    ASSERT(a == b);
}

static void rendered_equality_different() {
    RenderedPosition a;
    a.sample = 100;
    a.frames_written = 512;

    RenderedPosition b;
    b.sample = 200;
    b.frames_written = 512;

    ASSERT(!(a == b));
}

// =============================================================================
// Runner
// =============================================================================
void run_position_tests(ContractTestRunner& runner) {
    std::cout << "--- DecodedPosition ---\n";
    runner.run("decoded_default_construction_is_invalid", decoded_default_construction_is_invalid);
    runner.run("decoded_valid_construction", decoded_valid_construction);
    runner.run("decoded_invalid_sample", decoded_invalid_sample);
    runner.run("decoded_equality_same", decoded_equality_same);
    runner.run("decoded_equality_different", decoded_equality_different);
    std::cout << "\n";

    std::cout << "--- RenderedPosition ---\n";
    runner.run("rendered_default_construction_is_invalid", rendered_default_construction_is_invalid);
    runner.run("rendered_valid_construction", rendered_valid_construction);
    runner.run("rendered_invalid_sample", rendered_invalid_sample);
    runner.run("rendered_equality_same", rendered_equality_same);
    runner.run("rendered_equality_different", rendered_equality_different);
    std::cout << "\n";
}
