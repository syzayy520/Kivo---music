#include <exception>
#include <iostream>
#include <iterator>

#include "playback_session_test_cases.hpp"

int main() {
    using namespace playback_session_test;
    struct Test {
        const char* name;
        void (*run)();
    };
    const Test tests[] = {
        {"lifecycle_and_pause_freeze", lifecycle_and_pause_freeze},
        {"replacement_rejects_old_session_commands",
         replacement_rejects_old_session_commands},
        {"command_ids_are_globally_monotonic",
         command_ids_are_globally_monotonic},
        {"failed_session_can_close_and_shutdown",
         failed_session_can_close_and_shutdown},
        {"concurrent_intake_remains_serialized",
         concurrent_intake_remains_serialized},
        {"drain_completion_is_generation_safe",
         drain_completion_is_generation_safe},
        {"drain_failure_enters_failed_state",
         drain_failure_enters_failed_state},
        {"drain_command_races_are_deterministic",
         drain_command_races_are_deterministic},
        {"rapid_seek_coalesces_and_restores_state",
         rapid_seek_coalesces_and_restores_state},
        {"paused_seek_stays_paused_and_flush_advances",
         paused_seek_stays_paused_and_flush_advances},
        {"recovery_policy_is_classified_and_generation_scoped",
         recovery_policy_is_classified_and_generation_scoped},
        {"recovery_interrupts_seek_without_restoring_stale_intent",
         recovery_interrupts_seek_without_restoring_stale_intent},
        {"new_source_clears_previous_error_identity",
         new_source_clears_previous_error_identity},
        {"recovery_executor_primary_success_restores_playback",
         recovery_executor_primary_success_restores_playback},
        {"recovery_executor_fallback_can_stop_safely",
         recovery_executor_fallback_can_stop_safely},
        {"recovery_executor_double_failure_enters_failed",
         recovery_executor_double_failure_enters_failed},
        {"recovery_executor_rejections_execute_nothing",
         recovery_executor_rejections_execute_nothing}
    };

    int passed = 0;
    std::cout << "=== Playback Session Tests ===\n\n";
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
