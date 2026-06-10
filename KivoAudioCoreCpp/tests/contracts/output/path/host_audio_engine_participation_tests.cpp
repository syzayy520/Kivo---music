// =============================================================================
// KivoAudioCoreCpp — host_audio_engine_participation_tests.cpp
// Tests for HostAudioEngineParticipation contract type
// =============================================================================

#include "kivo/core/contract/output/path/host_audio_engine_participation.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void default_engine_in_path() {
    HostAudioEngineParticipation ep;
    ASSERT(!ep.engine_in_path);
    ASSERT(ep.is_bypassing_engine());
}

static void engine_active() {
    HostAudioEngineParticipation ep;
    ep.engine_in_path = true;
    ASSERT(!ep.is_bypassing_engine());
}

static void equality_same() {
    HostAudioEngineParticipation a;
    a.engine_in_path = true;
    HostAudioEngineParticipation b = a;
    ASSERT(a == b);
}

static void equality_different() {
    HostAudioEngineParticipation a;
    a.engine_in_path = true;
    HostAudioEngineParticipation b;
    ASSERT(!(a == b));
}

void run_host_audio_engine_participation_tests(ContractTestRunner& runner) {
    std::cout << "--- HostAudioEngineParticipation ---\n";
    runner.run("default_engine_in_path", default_engine_in_path);
    runner.run("engine_active", engine_active);
    runner.run("equality_same", equality_same);
    runner.run("equality_different", equality_different);
    std::cout << "\n";
}
