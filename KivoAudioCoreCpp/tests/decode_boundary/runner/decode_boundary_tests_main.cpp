#include "../fixture/decode_boundary_test_runner.hpp"

void run_decoded_block_contract_tests(DecodeBoundaryTestRunner& runner);
void run_decode_control_result_contract_tests(DecodeBoundaryTestRunner& runner);
void run_media_probe_contract_tests(DecodeBoundaryTestRunner& runner);
void run_decode_open_result_contract_tests(DecodeBoundaryTestRunner& runner);
void run_decode_step_result_contract_tests(DecodeBoundaryTestRunner& runner);
void run_large_file_byte_source_tests(DecodeBoundaryTestRunner& runner);
void run_local_file_failure_tests(DecodeBoundaryTestRunner& runner);
void run_local_file_open_tests(DecodeBoundaryTestRunner& runner);
void run_local_file_read_tests(DecodeBoundaryTestRunner& runner);
void run_local_file_seek_tests(DecodeBoundaryTestRunner& runner);
void run_media_support_tests(DecodeBoundaryTestRunner& runner);

int main() {
    std::cout << "=== Decode Boundary Tests ===\n\n";

    DecodeBoundaryTestRunner runner;
    run_media_probe_contract_tests(runner);
    run_decode_open_result_contract_tests(runner);
    run_decoded_block_contract_tests(runner);
    run_decode_step_result_contract_tests(runner);
    run_decode_control_result_contract_tests(runner);
    run_large_file_byte_source_tests(runner);
    run_local_file_open_tests(runner);
    run_local_file_read_tests(runner);
    run_local_file_seek_tests(runner);
    run_local_file_failure_tests(runner);
    run_media_support_tests(runner);

    std::cout << "\n=== " << runner.tests_passed << "/" << runner.tests_run
              << " passed ===\n";
    return runner.exit_code();
}
