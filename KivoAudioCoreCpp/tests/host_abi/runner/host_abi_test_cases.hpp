#pragma once

namespace host_abi_test {

void c_header_is_usable();
void versioned_structs_accept_tail_extension();
void legacy_snapshot_prefix_is_not_overwritten();
void extended_diagnostic_snapshot_is_advertised();
void invalid_and_stale_handles_are_rejected();
void shutdown_is_terminal_and_destroy_remains_valid();
void accepted_source_ownership_releases_once();

} // namespace host_abi_test
