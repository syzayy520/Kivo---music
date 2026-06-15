#include "render_queue_tests_main.hpp"

void run_spsc_audio_block_queue_tests(RenderQueueTestRunner& runner);
void run_render_pump_tests(RenderQueueTestRunner& runner);

int main() {
    std::cout << "=== Render Queue Tests ===\n\n";

    RenderQueueTestRunner runner;
    run_spsc_audio_block_queue_tests(runner);
    run_render_pump_tests(runner);

    std::cout << "\n=== " << runner.tests_passed << "/" << runner.tests_run
              << " passed ===\n";
    return runner.exit_code();
}
