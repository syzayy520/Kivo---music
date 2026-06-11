#include "decode_boundary_tests_main.hpp"

void run_decode_contract_tests(DecodeBoundaryTestRunner& runner);
void run_local_file_byte_source_tests(DecodeBoundaryTestRunner& runner);

int main() {
    std::cout << "=== Decode Boundary Tests ===\n\n";

    DecodeBoundaryTestRunner runner;
    run_decode_contract_tests(runner);
    run_local_file_byte_source_tests(runner);

    std::cout << "\n=== " << runner.tests_passed << "/" << runner.tests_run
              << " passed ===\n";
    return runner.exit_code();
}
