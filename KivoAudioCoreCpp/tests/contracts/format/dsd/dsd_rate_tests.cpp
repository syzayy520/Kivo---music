#include "kivo/core/contract/format/dsd/dsd_rate.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void known_rates_have_multipliers() {
    ASSERT(dsd_rate_multiplier(DsdRate::Dsd64) == 64);
    ASSERT(dsd_rate_multiplier(DsdRate::Dsd128) == 128);
    ASSERT(dsd_rate_multiplier(DsdRate::Dsd256) == 256);
    ASSERT(dsd_rate_multiplier(DsdRate::Dsd512) == 512);
}

static void bit_rate_uses_44100_family() {
    ASSERT(dsd_bit_rate(DsdRate::Dsd64) == 2822400);
    ASSERT(dsd_bit_rate(DsdRate::Dsd128) == 5644800);
}

static void unknown_rate_is_not_known() {
    ASSERT(!dsd_rate_is_known(DsdRate::Unknown));
    ASSERT(dsd_bit_rate(DsdRate::Unknown) == 0);
}

void run_dsd_rate_tests(ContractTestRunner& runner) {
    std::cout << "--- DsdRate ---\n";
    runner.run("known_rates_have_multipliers", known_rates_have_multipliers);
    runner.run("bit_rate_uses_44100_family", bit_rate_uses_44100_family);
    runner.run("unknown_rate_is_not_known", unknown_rate_is_not_known);
    std::cout << "\n";
}
