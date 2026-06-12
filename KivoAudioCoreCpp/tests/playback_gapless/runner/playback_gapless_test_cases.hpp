#pragma once

namespace playback_gapless_test {

void compatible_tracks_share_exact_boundary();
void media_probe_builds_trim_aware_track_plan();
void transition_rejections_are_explicit();
void consecutive_tracks_preserve_pcm_and_eos();
void coordinator_prefetches_and_advances_without_queue_gap();
void coordinator_rejects_incompatible_prefetch();
void coordinator_rejects_successor_after_final_track();
void coordinator_requires_prefetched_non_final_successor();
void position_frontiers_are_ordered();
void whole_file_segments_map_exactly();
void malformed_whole_file_segments_are_rejected();

} // namespace playback_gapless_test
