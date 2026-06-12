#include "../fixture/wasapi_test_runner.hpp"
#include "wasapi_test_cases.hpp"


int main() {
    std::cout << "=== WASAPI Adapter Tests ===\n\n";

    WasapiTestRunner runner;
    run_wasapi_format_mapping_tests(runner);
    run_wasapi_error_mapping_tests(runner);
    run_wasapi_endpoint_notification_tests(runner);
    run_wasapi_render_worker_tests(runner);

    std::cout << "\n=== " << runner.tests_passed << "/" << runner.tests_run << " passed ===\n";
    return runner.exit_code();
}
