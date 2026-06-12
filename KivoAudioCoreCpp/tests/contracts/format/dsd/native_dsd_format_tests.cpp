#include "kivo/core/contract/format/dsd/native_dsd_format.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void native_dsd_format_requires_known_rate_and_channels() {
    NativeDsdFormat format{DsdRate::Dsd64, 2};
    ASSERT(format.is_valid());
    ASSERT(format.bit_rate() == 2822400);
}

static void native_dsd_format_rejects_unknown_rate() {
    NativeDsdFormat format{DsdRate::Unknown, 2};
    ASSERT(!format.is_valid());
}

static void native_dsd_format_rejects_invalid_channel_count() {
    NativeDsdFormat no_channels{DsdRate::Dsd64, 0};
    NativeDsdFormat too_many{DsdRate::Dsd64, 9};
    ASSERT(!no_channels.is_valid());
    ASSERT(!too_many.is_valid());
}

void run_native_dsd_format_tests(ContractTestRunner& runner) {
    std::cout << "--- NativeDsdFormat ---\n";
    runner.run("native_dsd_format_requires_known_rate_and_channels",
               native_dsd_format_requires_known_rate_and_channels);
    runner.run("native_dsd_format_rejects_unknown_rate",
               native_dsd_format_rejects_unknown_rate);
    runner.run("native_dsd_format_rejects_invalid_channel_count",
               native_dsd_format_rejects_invalid_channel_count);
    std::cout << "\n";
}
