#include <iostream>
#include <iterator>

#include "playback_gapless_test_cases.hpp"

int main() {
    using namespace playback_gapless_test;
    struct Test {
        const char* name;
        void (*run)();
    };
    const Test tests[] = {
        {"compatible_tracks_share_exact_boundary",
         compatible_tracks_share_exact_boundary},
        {"media_probe_builds_trim_aware_track_plan",
         media_probe_builds_trim_aware_track_plan},
        {"transition_rejections_are_explicit",
         transition_rejections_are_explicit},
        {"consecutive_tracks_preserve_pcm_and_eos",
         consecutive_tracks_preserve_pcm_and_eos},
        {"coordinator_prefetches_and_advances_without_queue_gap",
         coordinator_prefetches_and_advances_without_queue_gap},
        {"coordinator_rejects_incompatible_prefetch",
         coordinator_rejects_incompatible_prefetch},
        {"coordinator_rejects_successor_after_final_track",
         coordinator_rejects_successor_after_final_track},
        {"coordinator_requires_prefetched_non_final_successor",
         coordinator_requires_prefetched_non_final_successor},
        {"position_frontiers_are_ordered",
         position_frontiers_are_ordered},
        {"whole_file_segments_map_exactly",
         whole_file_segments_map_exactly},
        {"malformed_whole_file_segments_are_rejected",
         malformed_whole_file_segments_are_rejected}
    };
    int passed = 0;
    std::cout << "=== Playback Gapless Tests ===\n\n";
    for (const auto& test : tests) {
        std::cout << "  " << test.name << "... ";
        try {
            test.run();
            ++passed;
            std::cout << "PASS\n";
        } catch (const std::exception& exception) {
            std::cout << "FAIL: " << exception.what() << "\n";
        }
    }
    std::cout << "\n=== " << passed << "/" << std::size(tests)
              << " passed ===\n";
    return passed == std::size(tests) ? 0 : 1;
}
