// =============================================================================
// KivoAudioCoreCpp — processing_participation_tests.cpp
// Tests for ProcessingParticipation contract type
// =============================================================================

#include "kivo/core/contract/output/processing/processing_participation.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void default_has_no_processing() {
    ProcessingParticipation pp;
    ASSERT(!pp.any_processing_active());
    ASSERT(pp.is_passthrough());
}

static void replay_gain_active() {
    ProcessingParticipation pp;
    pp.replay_gain_active = true;
    ASSERT(pp.any_processing_active());
    ASSERT(!pp.is_passthrough());
}

static void all_processing_active() {
    ProcessingParticipation pp;
    pp.replay_gain_active = true;
    pp.volume_active = true;
    pp.dsp_active = true;
    pp.channel_remix_active = true;
    ASSERT(pp.any_processing_active());
    ASSERT(!pp.is_passthrough());
}

static void equality_same() {
    ProcessingParticipation a;
    a.dsp_active = true;
    ProcessingParticipation b = a;
    ASSERT(a == b);
}

static void equality_different() {
    ProcessingParticipation a;
    a.replay_gain_active = true;
    ProcessingParticipation b;
    ASSERT(!(a == b));
}

void run_processing_participation_tests(ContractTestRunner& runner) {
    std::cout << "--- ProcessingParticipation ---\n";
    runner.run("default_has_no_processing", default_has_no_processing);
    runner.run("replay_gain_active", replay_gain_active);
    runner.run("all_processing_active", all_processing_active);
    runner.run("equality_same", equality_same);
    runner.run("equality_different", equality_different);
    std::cout << "\n";
}
