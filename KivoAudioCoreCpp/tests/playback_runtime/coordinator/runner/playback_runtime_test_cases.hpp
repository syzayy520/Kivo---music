#pragma once

namespace playback_runtime_test {

void open_failure_rolls_back_every_boundary();
void renderer_format_rejection_records_negotiation_failure();
void format_renegotiation_metrics_are_monotonic();
void processing_policy_is_propagated_and_bit_perfect_conflicts_reject();
void shutdown_waits_for_active_write_and_releases_runtime();
void close_waits_for_active_drain_and_releases_runtime();
void device_loss_during_flush_recovers_pipeline();
void stale_pause_cannot_mutate_replaced_source();
void source_failure_stops_media_with_classified_domain();
void codec_failure_is_truthful_and_remains_closable();
void eos_drains_buffer_and_stops_session();
void drain_timeout_fails_and_close_releases_boundaries();
void device_loss_reopens_pipeline_and_resumes();
void failed_device_recovery_remains_closable();
void seek_resets_all_scopes_and_discards_old_audio();
void stale_command_cannot_mutate_renderer();
void failed_seek_can_be_closed_and_shutdown_is_terminal();

} // namespace playback_runtime_test
