#pragma once

namespace playback_runtime_test {

void open_failure_rolls_back_every_boundary();
void eos_drains_buffer_and_stops_session();
void drain_timeout_fails_and_close_releases_boundaries();
void seek_resets_all_scopes_and_discards_old_audio();
void stale_command_cannot_mutate_renderer();
void failed_seek_can_be_closed_and_shutdown_is_terminal();

} // namespace playback_runtime_test
