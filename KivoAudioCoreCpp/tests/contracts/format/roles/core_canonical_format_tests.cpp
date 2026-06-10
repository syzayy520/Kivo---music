// =============================================================================
// KivoAudioCoreCpp — core_canonical_format_tests.cpp
// Tests for CoreCanonicalFormat contract type
// =============================================================================

#include "kivo/core/contract/format/roles/core_canonical_format.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void default_format_is_invalid() {
    CoreCanonicalFormat ccf;
    ASSERT(!ccf.is_valid());
}

static void valid_format() {
    CoreCanonicalFormat ccf;
    ccf.format.sample_format = SampleFormat::Float32;
    ccf.format.channel_layout = ChannelLayout::Stereo;
    ccf.format.sample_rate = 48000;
    ccf.format.bits_per_sample = 32;
    ASSERT(ccf.is_valid());
}

static void equality_same() {
    CoreCanonicalFormat a;
    a.format.sample_format = SampleFormat::Float32;
    a.format.sample_rate = 48000;
    a.format.bits_per_sample = 32;

    CoreCanonicalFormat b = a;
    ASSERT(a == b);
}

static void equality_different_rate() {
    CoreCanonicalFormat a;
    a.format.sample_rate = 44100;

    CoreCanonicalFormat b;
    b.format.sample_rate = 48000;

    ASSERT(!(a == b));
}

void run_core_canonical_format_contract_tests(ContractTestRunner& runner) {
    std::cout << "--- CoreCanonicalFormat ---\n";
    runner.run("default_format_is_invalid", default_format_is_invalid);
    runner.run("valid_format", valid_format);
    runner.run("equality_same", equality_same);
    runner.run("equality_different_rate", equality_different_rate);
    std::cout << "\n";
}
