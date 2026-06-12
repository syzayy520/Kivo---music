#include <iostream>
#include <iterator>

#include "playback_runtime_test_cases.hpp"

int main() {
    using namespace playback_runtime_test;
    struct Test {
        const char* name;
        void (*run)();
    };
    const Test tests[] = {
        {"open_failure_rolls_back_every_boundary",
         open_failure_rolls_back_every_boundary},
        {"seek_resets_all_scopes_and_discards_old_audio",
         seek_resets_all_scopes_and_discards_old_audio},
        {"stale_command_cannot_mutate_renderer",
         stale_command_cannot_mutate_renderer},
        {"failed_seek_can_be_closed_and_shutdown_is_terminal",
         failed_seek_can_be_closed_and_shutdown_is_terminal}
    };

    int passed = 0;
    std::cout << "=== Playback Runtime Coordinator Tests ===\n\n";
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
