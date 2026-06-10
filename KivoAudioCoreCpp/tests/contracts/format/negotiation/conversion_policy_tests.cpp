// =============================================================================
// KivoAudioCoreCpp — conversion_policy_tests.cpp
// Tests for ConversionPolicy contract type
// =============================================================================

#include "kivo/core/contract/format/negotiation/conversion_policy.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void default_policy_has_no_conversions() {
    ConversionPolicy policy;
    ASSERT(!policy.any_conversion_needed());
    ASSERT(policy.is_passthrough());
}

static void sample_format_convert_needs_conversion() {
    ConversionPolicy policy;
    policy.sample_format_convert = true;
    ASSERT(policy.any_conversion_needed());
    ASSERT(!policy.is_passthrough());
}

static void sample_rate_convert_needs_conversion() {
    ConversionPolicy policy;
    policy.sample_rate_convert = true;
    ASSERT(policy.any_conversion_needed());
}

static void channel_remix_needs_conversion() {
    ConversionPolicy policy;
    policy.channel_remix = true;
    ASSERT(policy.any_conversion_needed());
}

static void bit_depth_convert_needs_conversion() {
    ConversionPolicy policy;
    policy.bit_depth_convert = true;
    ASSERT(policy.any_conversion_needed());
}

static void all_conversions_set() {
    ConversionPolicy policy;
    policy.sample_format_convert = true;
    policy.sample_rate_convert = true;
    policy.channel_remix = true;
    policy.bit_depth_convert = true;
    ASSERT(policy.any_conversion_needed());
    ASSERT(!policy.is_passthrough());
}

static void equality_same() {
    ConversionPolicy a;
    a.sample_rate_convert = true;

    ConversionPolicy b = a;
    ASSERT(a == b);
}

static void equality_different() {
    ConversionPolicy a;
    ConversionPolicy b;
    b.sample_rate_convert = true;

    ASSERT(!(a == b));
}

void run_conversion_policy_contract_tests(ContractTestRunner& runner) {
    std::cout << "--- ConversionPolicy ---\n";
    runner.run("default_policy_has_no_conversions", default_policy_has_no_conversions);
    runner.run("sample_format_convert_needs_conversion", sample_format_convert_needs_conversion);
    runner.run("sample_rate_convert_needs_conversion", sample_rate_convert_needs_conversion);
    runner.run("channel_remix_needs_conversion", channel_remix_needs_conversion);
    runner.run("bit_depth_convert_needs_conversion", bit_depth_convert_needs_conversion);
    runner.run("all_conversions_set", all_conversions_set);
    runner.run("equality_same", equality_same);
    runner.run("equality_different", equality_different);
    std::cout << "\n";
}
