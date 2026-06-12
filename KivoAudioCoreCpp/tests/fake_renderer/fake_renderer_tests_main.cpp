#include "fake_renderer_tests_main.hpp"

void run_manual_clock_tests(FakeRendererTestRunner& runner);
void run_fake_renderer_lifecycle_tests(FakeRendererTestRunner& runner);
void run_fake_renderer_write_generation_tests(FakeRendererTestRunner& runner);
void run_fake_renderer_fault_injection_tests(FakeRendererTestRunner& runner);
void run_fake_renderer_device_lifecycle_fault_tests(
    FakeRendererTestRunner& runner);
void run_fake_renderer_observation_history_tests(FakeRendererTestRunner& runner);

int main() {
    std::cout << "=== Fake Renderer Tests ===\n\n";

    FakeRendererTestRunner runner;
    run_manual_clock_tests(runner);
    run_fake_renderer_lifecycle_tests(runner);
    run_fake_renderer_write_generation_tests(runner);
    run_fake_renderer_fault_injection_tests(runner);
    run_fake_renderer_device_lifecycle_fault_tests(runner);
    run_fake_renderer_observation_history_tests(runner);

    std::cout << "\n=== " << runner.tests_passed << "/" << runner.tests_run << " passed ===\n";
    return runner.exit_code();
}
