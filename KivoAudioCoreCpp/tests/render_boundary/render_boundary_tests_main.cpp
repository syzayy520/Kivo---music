#include "render_boundary_tests_main.hpp"

void run_renderer_identity_capability_tests(RenderBoundaryTestRunner& runner);
void run_render_format_lifecycle_tests(RenderBoundaryTestRunner& runner);
void run_audio_block_view_tests(RenderBoundaryTestRunner& runner);
void run_render_write_result_tests(RenderBoundaryTestRunner& runner);
void run_render_boundary_interface_tests(RenderBoundaryTestRunner& runner);

int main() {
    std::cout << "=== Render Boundary Tests ===\n\n";

    RenderBoundaryTestRunner runner;
    run_renderer_identity_capability_tests(runner);
    run_render_format_lifecycle_tests(runner);
    run_audio_block_view_tests(runner);
    run_render_write_result_tests(runner);
    run_render_boundary_interface_tests(runner);

    std::cout << "\n=== " << runner.tests_passed << "/" << runner.tests_run << " passed ===\n";
    return runner.exit_code();
}
