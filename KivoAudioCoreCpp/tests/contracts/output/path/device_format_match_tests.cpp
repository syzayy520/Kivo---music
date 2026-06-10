// =============================================================================
// KivoAudioCoreCpp — device_format_match_tests.cpp
// Tests for DeviceFormatMatch contract type
// =============================================================================

#include "kivo/core/contract/output/path/device_format_match.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void default_is_not_match() {
    DeviceFormatMatch match;
    ASSERT(!match.is_match());
    ASSERT(!match.formats_equal);
}

static void formats_are_equal() {
    DeviceFormatMatch match;
    match.formats_equal = true;
    ASSERT(match.is_match());
}

static void optional_source_format() {
    DeviceFormatMatch match;
    AudioFormatDescriptor fmt;
    fmt.sample_format = SampleFormat::Int16;
    fmt.sample_rate = 44100;
    fmt.bits_per_sample = 16;
    match.source_format = fmt;
    ASSERT(match.source_format.has_value());
}

static void equality_same() {
    DeviceFormatMatch a;
    a.formats_equal = true;
    DeviceFormatMatch b = a;
    ASSERT(a == b);
}

static void equality_different() {
    DeviceFormatMatch a;
    a.formats_equal = true;
    DeviceFormatMatch b;
    b.formats_equal = false;
    ASSERT(!(a == b));
}

void run_device_format_match_tests(ContractTestRunner& runner) {
    std::cout << "--- DeviceFormatMatch ---\n";
    runner.run("default_is_not_match", default_is_not_match);
    runner.run("formats_are_equal", formats_are_equal);
    runner.run("optional_source_format", optional_source_format);
    runner.run("equality_same", equality_same);
    runner.run("equality_different", equality_different);
    std::cout << "\n";
}
