// =============================================================================
// KivoAudioCoreCpp — format_negotiation_result_tests.cpp
// Tests for FormatNegotiationResult contract type
// =============================================================================

#include "kivo/core/contract/format/negotiation/format_negotiation_result.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void default_result_has_no_conversions() {
    FormatNegotiationResult result;
    ASSERT(!result.resample_needed);
    ASSERT(!result.conversion_needed);
    ASSERT(!result.bit_perfect_possible);
}

static void formats_match_when_identical() {
    FormatNegotiationResult result;
    result.native_format.sample_format = SampleFormat::Int16;
    result.native_format.channel_layout = ChannelLayout::Stereo;
    result.native_format.sample_rate = 44100;
    result.native_format.bits_per_sample = 16;

    result.render_format = result.native_format;
    result.device_format = result.native_format;

    ASSERT(result.formats_match());
}

static void formats_dont_match_when_different() {
    FormatNegotiationResult result;
    result.native_format.sample_rate = 44100;
    result.render_format.sample_rate = 48000;
    result.device_format.sample_rate = 44100;

    ASSERT(!result.formats_match());
}

static void equality_same() {
    FormatNegotiationResult a;
    a.native_format.sample_rate = 44100;

    FormatNegotiationResult b = a;
    ASSERT(a == b);
}

static void equality_different() {
    FormatNegotiationResult a;
    a.native_format.sample_rate = 44100;

    FormatNegotiationResult b;
    b.native_format.sample_rate = 48000;

    ASSERT(!(a == b));
}

static void resample_flag_preserved() {
    FormatNegotiationResult result;
    result.resample_needed = true;
    ASSERT(result.resample_needed);

    result.conversion_needed = true;
    ASSERT(result.conversion_needed);
}

void run_format_negotiation_result_contract_tests(ContractTestRunner& runner) {
    std::cout << "--- FormatNegotiationResult ---\n";
    runner.run("default_result_has_no_conversions", default_result_has_no_conversions);
    runner.run("formats_match_when_identical", formats_match_when_identical);
    runner.run("formats_dont_match_when_different", formats_dont_match_when_different);
    runner.run("equality_same", equality_same);
    runner.run("equality_different", equality_different);
    runner.run("resample_flag_preserved", resample_flag_preserved);
    std::cout << "\n";
}
