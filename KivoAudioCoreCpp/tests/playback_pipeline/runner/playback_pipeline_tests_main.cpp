#include <iostream>
#include <iterator>

#include "playback_pipeline_test_cases.hpp"

int main() {
    using namespace playback_pipeline_test;
    struct Test {
        const char* name;
        void (*run)();
    };
    const Test tests[] = {
        {"lookahead_marks_only_final_block_eos",
         lookahead_marks_only_final_block_eos},
        {"chunking_preserves_offsets_and_terminal_tail",
         chunking_preserves_offsets_and_terminal_tail},
        {"non_final_track_keeps_queue_open_and_maps_timeline",
         non_final_track_keeps_queue_open_and_maps_timeline},
        {"timeline_offset_overflow_fails_before_queue_mutation",
         timeline_offset_overflow_fails_before_queue_mutation},
        {"full_queue_reports_backpressure_without_losing_pending",
         full_queue_reports_backpressure_without_losing_pending},
        {"oversized_decode_block_fails_before_queue_mutation",
         oversized_decode_block_fails_before_queue_mutation},
        {"creation_rejects_chunk_larger_than_queue_slot",
         creation_rejects_chunk_larger_than_queue_slot},
        {"stale_decode_generation_fails_before_queue_mutation",
         stale_decode_generation_fails_before_queue_mutation},
        {"stale_source_generation_fails_before_queue_mutation",
         stale_source_generation_fails_before_queue_mutation},
        {"processing_runs_before_queue_publication",
         processing_runs_before_queue_publication}
    };
    int passed = 0;
    std::cout << "=== Playback Pipeline Tests ===\n\n";
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
