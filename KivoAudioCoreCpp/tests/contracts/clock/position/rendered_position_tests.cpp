// =============================================================================
// KivoAudioCoreCpp — rendered_position_tests.cpp
// Tests for RenderedPosition contract type
// =============================================================================

#include "kivo/core/contract/clock/position/rendered_position.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// Tests
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
void run_rendered_position_tests(ContractTestRunner& runner) {
    std::cout << "--- RenderedPosition ---\n";
    runner.run("rendered_default_construction_is_invalid", rendered_default_construction_is_invalid);
    runner.run("rendered_valid_construction", rendered_valid_construction);
    runner.run("rendered_invalid_sample", rendered_invalid_sample);
    runner.run("rendered_equality_same", rendered_equality_same);
    runner.run("rendered_equality_different", rendered_equality_different);
    std::cout << "\n";
}
