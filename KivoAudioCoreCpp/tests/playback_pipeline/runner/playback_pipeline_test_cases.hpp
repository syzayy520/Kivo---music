#pragma once

namespace playback_pipeline_test {

void lookahead_marks_only_final_block_eos();
void chunking_preserves_offsets_and_terminal_tail();
void non_final_track_keeps_queue_open_and_maps_timeline();
void timeline_offset_overflow_fails_before_queue_mutation();
void full_queue_reports_backpressure_without_losing_pending();
void oversized_decode_block_fails_before_queue_mutation();
void creation_rejects_chunk_larger_than_queue_slot();
void stale_decode_generation_fails_before_queue_mutation();
void stale_source_generation_fails_before_queue_mutation();

} // namespace playback_pipeline_test
