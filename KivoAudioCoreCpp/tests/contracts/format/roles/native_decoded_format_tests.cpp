// =============================================================================
// KivoAudioCoreCpp — native_decoded_format_tests.cpp
// Tests for NativeDecodedFormat contract type
// =============================================================================

#include "kivo/core/contract/format/roles/native_decoded_format.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void default_format_is_invalid() {
    NativeDecodedFormat ndf;
    ASSERT(!ndf.is_valid());
}

static void valid_format() {
    NativeDecodedFormat ndf;
    ndf.format.sample_format = SampleFormat::Int16;
    ndf.format.channel_layout = ChannelLayout::Stereo;
    ndf.format.sample_rate = 44100;
    ndf.format.bits_per_sample = 16;
    ASSERT(ndf.is_valid());
}

static void equality_same() {
    NativeDecodedFormat a;
    a.format.sample_format = SampleFormat::Int16;
    a.format.sample_rate = 44100;
    a.format.bits_per_sample = 16;

    NativeDecodedFormat b = a;
    ASSERT(a == b);
}

static void equality_different() {
    NativeDecodedFormat a;
    a.format.sample_rate = 44100;

    NativeDecodedFormat b;
    b.format.sample_rate = 48000;

    ASSERT(!(a == b));
}

void run_native_decoded_format_contract_tests(ContractTestRunner& runner) {
    std::cout << "--- NativeDecodedFormat ---\n";
    runner.run("default_format_is_invalid", default_format_is_invalid);
    runner.run("valid_format", valid_format);
    runner.run("equality_same", equality_same);
    runner.run("equality_different", equality_different);
    std::cout << "\n";
}
