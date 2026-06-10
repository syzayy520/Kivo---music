// =============================================================================
// KivoAudioCoreCpp — result_tests.cpp
// Tests for Result/StatusOr contract types
// =============================================================================

#include "kivo/core/contract/result.hpp"
#include <cassert>
#include <iostream>
#include <string>

using namespace kivo::core::contract;

// =============================================================================
// Test helpers
// =============================================================================
static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) \
    do { \
        tests_run++; \
        std::cout << "  " << #name << "... "; \
        try { \
            name(); \
            tests_passed++; \
            std::cout << "PASS\n"; \
        } catch (const std::exception& e) { \
            std::cout << "FAIL: " << e.what() << "\n"; \
        } \
    } while(0)

#define ASSERT(cond) \
    do { \
        if (!(cond)) { \
            throw std::runtime_error("Assertion failed: " #cond); \
        } \
    } while(0)

// =============================================================================
// Result tests
// =============================================================================
void result_ok_value() {
    ASSERT(Result::Ok != Result::Error);
}

void result_error_value() {
    ASSERT(Result::Error != Result::Ok);
}

// =============================================================================
// StatusOr tests
// =============================================================================
void statusor_ok_construction() {
    auto s = StatusOr<int>::Ok(42);
    ASSERT(s.is_ok());
    ASSERT(!s.is_error());
    ASSERT(s.result() == Result::Ok);
}

void statusor_error_construction() {
    auto s = StatusOr<int>::Error("fail");
    ASSERT(!s.is_ok());
    ASSERT(s.is_error());
    ASSERT(s.result() == Result::Error);
}

void statusor_ok_value_access() {
    auto s = StatusOr<int>::Ok(42);
    ASSERT(s.value() == 42);
}

void statusor_error_message_access() {
    auto s = StatusOr<int>::Error("something went wrong");
    ASSERT(s.error() == "something went wrong");
}

void statusor_string_value() {
    auto s = StatusOr<std::string>::Ok("hello");
    ASSERT(s.value() == "hello");
}

void statusor_equality() {
    auto a = StatusOr<int>::Ok(42);
    auto b = StatusOr<int>::Ok(42);
    auto c = StatusOr<int>::Error("fail");
    ASSERT(a == b);
    ASSERT(!(a == c));
}

// =============================================================================
// Main
// =============================================================================
int main() {
    std::cout << "result_tests:\n";

    TEST(result_ok_value);
    TEST(result_error_value);
    TEST(statusor_ok_construction);
    TEST(statusor_error_construction);
    TEST(statusor_ok_value_access);
    TEST(statusor_error_message_access);
    TEST(statusor_string_value);
    TEST(statusor_equality);

    std::cout << "\n  " << tests_passed << "/" << tests_run << " passed\n";
    return (tests_passed == tests_run) ? 0 : 1;
}
