#include "wasapi_tests_main.hpp"

void run_wasapi_format_mapping_tests(WasapiTestRunner& runner);
void run_wasapi_error_mapping_tests(WasapiTestRunner& runner);
void run_wasapi_render_worker_tests(WasapiTestRunner& runner);

int main() {
    std::cout << "=== WASAPI Adapter Tests ===\n\n";

    WasapiTestRunner runner;
    run_wasapi_format_mapping_tests(runner);
    run_wasapi_error_mapping_tests(runner);
    run_wasapi_render_worker_tests(runner);

    std::cout << "\n=== " << runner.tests_passed << "/" << runner.tests_run << " passed ===\n";
    return runner.exit_code();
}
