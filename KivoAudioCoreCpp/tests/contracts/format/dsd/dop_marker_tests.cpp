#include "kivo/core/contract/format/dsd/dop_marker.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void default_marker_pair_is_valid_dop_marker_pair() {
    DopMarkerPair markers{};
    ASSERT(markers.is_valid());
    ASSERT(markers.first == 0x05);
    ASSERT(markers.second == 0xFA);
}

static void changed_marker_pair_is_invalid() {
    DopMarkerPair markers{};
    markers.second = 0x00;
    ASSERT(!markers.is_valid());
}

static void either_alternating_phase_is_valid() {
    const DopMarkerPair markers{0xFA, 0x05};
    ASSERT(markers.is_valid());
}

void run_dop_marker_tests(ContractTestRunner& runner) {
    std::cout << "--- DopMarkerPair ---\n";
    runner.run("default_marker_pair_is_valid_dop_marker_pair",
               default_marker_pair_is_valid_dop_marker_pair);
    runner.run("changed_marker_pair_is_invalid", changed_marker_pair_is_invalid);
    runner.run("either_alternating_phase_is_valid",
               either_alternating_phase_is_valid);
    std::cout << "\n";
}
