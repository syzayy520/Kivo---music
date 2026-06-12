#include "kivo/core/contract/format/dsd/dop_carrier_format.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void dop_carrier_derives_pcm_carrier_rate_from_dsd_payload() {
    DopCarrierFormat carrier{DsdRate::Dsd64, 176400};
    ASSERT(carrier.expected_carrier_sample_rate() == 176400);
    ASSERT(carrier.is_valid());
}

static void dop_carrier_rejects_wrong_sample_rate() {
    DopCarrierFormat carrier{DsdRate::Dsd64, 192000};
    ASSERT(!carrier.is_valid());
}

static void dop_carrier_accepts_24_valid_bits_in_32_bit_container() {
    DopCarrierFormat carrier{DsdRate::Dsd64, 176400, 24, 32};
    ASSERT(carrier.is_valid());
}

static void dop_carrier_rejects_wrong_valid_bits() {
    DopCarrierFormat carrier{DsdRate::Dsd64, 176400, 16, 32};
    ASSERT(!carrier.is_valid());
}

static void dop_carrier_rejects_bad_markers() {
    DopCarrierFormat carrier{DsdRate::Dsd64, 176400};
    carrier.marker_pair = {0x00, 0xFA};
    ASSERT(!carrier.is_valid());
}

static void dop_carrier_requires_cadence_and_channel_sync() {
    DopCarrierFormat carrier{DsdRate::Dsd64, 176400};
    carrier.markers_alternate_per_sample = false;
    ASSERT(!carrier.is_valid());
    carrier.markers_alternate_per_sample = true;
    carrier.markers_synchronized_across_channels = false;
    ASSERT(!carrier.is_valid());
}

void run_dop_carrier_format_tests(ContractTestRunner& runner) {
    std::cout << "--- DopCarrierFormat ---\n";
    runner.run("dop_carrier_derives_pcm_carrier_rate_from_dsd_payload",
               dop_carrier_derives_pcm_carrier_rate_from_dsd_payload);
    runner.run("dop_carrier_rejects_wrong_sample_rate",
               dop_carrier_rejects_wrong_sample_rate);
    runner.run("dop_carrier_accepts_24_valid_bits_in_32_bit_container",
               dop_carrier_accepts_24_valid_bits_in_32_bit_container);
    runner.run("dop_carrier_rejects_wrong_valid_bits",
               dop_carrier_rejects_wrong_valid_bits);
    runner.run("dop_carrier_rejects_bad_markers",
               dop_carrier_rejects_bad_markers);
    runner.run("dop_carrier_requires_cadence_and_channel_sync",
               dop_carrier_requires_cadence_and_channel_sync);
    std::cout << "\n";
}
