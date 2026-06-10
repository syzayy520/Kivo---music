// =============================================================================
// KivoAudioCoreCpp — device_format_tests.cpp
// Tests for DeviceFormat contract type
// =============================================================================

#include "kivo/core/contract/format/roles/device_format.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void default_format_is_invalid() {
    DeviceFormat df;
    ASSERT(!df.is_valid());
}

static void valid_format() {
    DeviceFormat df;
    df.format.sample_format = SampleFormat::Int24;
    df.format.channel_layout = ChannelLayout::Surround51;
    df.format.sample_rate = 96000;
    df.format.bits_per_sample = 24;
    ASSERT(df.is_valid());
}

static void equality_same() {
    DeviceFormat a;
    a.format.sample_rate = 44100;

    DeviceFormat b = a;
    ASSERT(a == b);
}

static void equality_different() {
    DeviceFormat a;
    a.format.sample_rate = 44100;

    DeviceFormat b;
    b.format.sample_rate = 48000;

    ASSERT(!(a == b));
}

void run_device_format_contract_tests(ContractTestRunner& runner) {
    std::cout << "--- DeviceFormat ---\n";
    runner.run("default_format_is_invalid", default_format_is_invalid);
    runner.run("valid_format", valid_format);
    runner.run("equality_same", equality_same);
    runner.run("equality_different", equality_different);
    std::cout << "\n";
}
