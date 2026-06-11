#include "../../contract_tests_main.h"
#include "kivo/core/contract/error/severity/error_severity.hpp"

using namespace kivo::core::contract;

namespace {

void error_severity_default_is_unknown(ContractTestRunner& runner) {
    runner.run("error_severity_default_is_unknown", []() {
        ErrorSeverity s{};
        ASSERT(s == ErrorSeverity::Unknown);
    });
}

void error_severity_fatal(ContractTestRunner& runner) {
    runner.run("error_severity_fatal", []() {
        ASSERT(static_cast<uint8_t>(ErrorSeverity::Fatal) == 1);
    });
}

void error_severity_recoverable(ContractTestRunner& runner) {
    runner.run("error_severity_recoverable", []() {
        ASSERT(static_cast<uint8_t>(ErrorSeverity::Recoverable) == 2);
    });
}

void error_severity_warning(ContractTestRunner& runner) {
    runner.run("error_severity_warning", []() {
        ASSERT(static_cast<uint8_t>(ErrorSeverity::Warning) == 3);
    });
}

void error_severity_info(ContractTestRunner& runner) {
    runner.run("error_severity_info", []() {
        ASSERT(static_cast<uint8_t>(ErrorSeverity::Info) == 4);
    });
}

void error_severity_equality(ContractTestRunner& runner) {
    runner.run("error_severity_equality", []() {
        ASSERT(ErrorSeverity::Fatal == ErrorSeverity::Fatal);
        ASSERT(ErrorSeverity::Unknown != ErrorSeverity::Fatal);
    });
}

} // namespace

void run_error_severity_tests(ContractTestRunner& runner) {
    error_severity_default_is_unknown(runner);
    error_severity_fatal(runner);
    error_severity_recoverable(runner);
    error_severity_warning(runner);
    error_severity_info(runner);
    error_severity_equality(runner);
}