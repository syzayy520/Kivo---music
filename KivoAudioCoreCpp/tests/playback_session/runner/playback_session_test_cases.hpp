#pragma once

namespace playback_session_test {

void lifecycle_and_pause_freeze();
void replacement_rejects_old_session_commands();
void command_ids_are_globally_monotonic();
void failed_session_can_close_and_shutdown();
void concurrent_intake_remains_serialized();
void drain_completion_is_generation_safe();
void drain_failure_enters_failed_state();
void drain_command_races_are_deterministic();
void rapid_seek_coalesces_and_restores_state();
void paused_seek_stays_paused_and_flush_advances();
void recovery_policy_is_classified_and_generation_scoped();
void recovery_interrupts_seek_without_restoring_stale_intent();
void new_source_clears_previous_error_identity();
void recovery_executor_primary_success_restores_playback();
void recovery_executor_fallback_can_stop_safely();
void recovery_executor_double_failure_enters_failed();
void recovery_executor_rejections_execute_nothing();

} // namespace playback_session_test
