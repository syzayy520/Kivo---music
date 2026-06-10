// =============================================================================
// KivoAudioCoreCpp — render_format_tests.cpp
// Tests for RenderFormat contract type (audio render boundary, NOT video)
// =============================================================================

#include "kivo/core/contract/format/roles/render_format.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void default_format_is_invalid() {
    RenderFormat rf;
    ASSERT(!rf.is_valid());
}

static void valid_format() {
    RenderFormat rf;
    rf.format.sample_format = SampleFormat::Int16;
    rf.format.channel_layout = ChannelLayout::Stereo;
    rf.format.sample_rate = 44100;
    rf.format.bits_per_sample = 16;
    ASSERT(rf.is_valid());
}

static void equality_same() {
    RenderFormat a;
    a.format.sample_rate = 44100;

    RenderFormat b = a;
    ASSERT(a == b);
}

static void equality_different() {
    RenderFormat a;
    a.format.sample_rate = 44100;

    RenderFormat b;
    b.format.sample_rate = 48000;

    ASSERT(!(a == b));
}

void run_render_format_contract_tests(ContractTestRunner& runner) {
    std::cout << "--- RenderFormat (audio) ---\n";
    runner.run("default_format_is_invalid", default_format_is_invalid);
    runner.run("valid_format", valid_format);
    runner.run("equality_same", equality_same);
    runner.run("equality_different", equality_different);
    std::cout << "\n";
}
