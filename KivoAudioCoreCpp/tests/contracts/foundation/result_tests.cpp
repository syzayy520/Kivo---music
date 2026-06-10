// =============================================================================
// KivoAudioCoreCpp — result_tests.cpp
// Tests for Result/StatusOr contract types
// =============================================================================

#include "kivo/core/contract/result.hpp"
#include "../contract_tests_main.h"
#include <string>

using namespace kivo::core::contract;

// =============================================================================
// Tests
// =============================================================================
static void result_ok_value() {
    ASSERT(Result::Ok != Result::Error);
}

static void result_error_value() {
    ASSERT(Result::Error != Result::Ok);
}

static void statusor_ok_construction() {
    auto s = StatusOr<int>::Ok(42);
    ASSERT(s.is_ok());
    ASSERT(!s.is_error());
    ASSERT(s.result() == Result::Ok);
}

static void statusor_error_construction() {
    auto s = StatusOr<int>::Error("fail");
    ASSERT(!s.is_ok());
    ASSERT(s.is_error());
    ASSERT(s.result() == Result::Error);
}

static void statusor_ok_value_access() {
    auto s = StatusOr<int>::Ok(42);
    ASSERT(s.value() == 42);
}

static void statusor_error_message_access() {
    auto s = StatusOr<int>::Error("something went wrong");
    ASSERT(s.error() == "something went wrong");
}

static void statusor_string_value() {
    auto s = StatusOr<std::string>::Ok("hello");
    ASSERT(s.value() == "hello");
}

static void statusor_equality() {
    auto a = StatusOr<int>::Ok(42);
    auto b = StatusOr<int>::Ok(42);
    auto c = StatusOr<int>::Error("fail");
    ASSERT(a == b);
    ASSERT(!(a == c));
}

// =============================================================================
// Runner
// =============================================================================
void run_result_contract_tests(ContractTestRunner& runner) {
    std::cout << "--- Result/StatusOr ---\n";
    runner.run("result_ok_value", result_ok_value);
    runner.run("result_error_value", result_error_value);
    runner.run("statusor_ok_construction", statusor_ok_construction);
    runner.run("statusor_error_construction", statusor_error_construction);
    runner.run("statusor_ok_value_access", statusor_ok_value_access);
    runner.run("statusor_error_message_access", statusor_error_message_access);
    runner.run("statusor_string_value", statusor_string_value);
    runner.run("statusor_equality", statusor_equality);
    std::cout << "\n";
}
