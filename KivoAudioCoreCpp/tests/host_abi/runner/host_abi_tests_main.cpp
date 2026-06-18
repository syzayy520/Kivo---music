#include "host_abi_test_cases.hpp"

#include <iostream>

int main() {
    using namespace host_abi_test;
    c_header_is_usable();
    versioned_structs_accept_tail_extension();
    legacy_snapshot_prefix_is_not_overwritten();
    extended_diagnostic_snapshot_is_advertised();
    bit_perfect_truth_is_advertised_and_honest_when_idle();
    truth_getter_rejects_invalid_struct();
    invalid_and_stale_handles_are_rejected();
    shutdown_is_terminal_and_destroy_remains_valid();
    accepted_source_ownership_releases_once();
    std::cout << "host ABI tests passed\n";
    return 0;
}
