#include "../../contract_tests_main.h"
#include "kivo/core/contract/error/severity/error_attributes.hpp"

using namespace kivo::core::contract;

namespace {

void safe_cleanup_path_default_is_none(ContractTestRunner& runner) {
    runner.run("safe_cleanup_path_default_is_none", []() {
        SafeCleanupPath p{};
        ASSERT(p == SafeCleanupPath::None);
    });
}

void safe_cleanup_path_graceful_drain(ContractTestRunner& runner) {
    runner.run("safe_cleanup_path_graceful_drain", []() {
        ASSERT(static_cast<uint8_t>(SafeCleanupPath::GracefulDrain) == 1);
    });
}

void safe_cleanup_path_immediate_flush(ContractTestRunner& runner) {
    runner.run("safe_cleanup_path_immediate_flush", []() {
        ASSERT(static_cast<uint8_t>(SafeCleanupPath::ImmediateFlush) == 2);
    });
}

void safe_cleanup_path_force_close(ContractTestRunner& runner) {
    runner.run("safe_cleanup_path_force_close", []() {
        ASSERT(static_cast<uint8_t>(SafeCleanupPath::ForceClose) == 3);
    });
}

void safe_cleanup_path_equality(ContractTestRunner& runner) {
    runner.run("safe_cleanup_path_equality", []() {
        ASSERT(SafeCleanupPath::GracefulDrain == SafeCleanupPath::GracefulDrain);
        ASSERT(SafeCleanupPath::None != SafeCleanupPath::GracefulDrain);
    });
}

void error_attributes_default_construction(ContractTestRunner& runner) {
    runner.run("error_attributes_default_construction", []() {
        ErrorAttributes a{};
        ASSERT(a.severity == ErrorSeverity::Unknown);
        ASSERT(a.recoverable == false);
        ASSERT(a.retryable == false);
        ASSERT(a.requires_reopen_device == false);
        ASSERT(a.requires_rebuild_codec == false);
        ASSERT(a.requires_user_action == false);
        ASSERT(a.safe_to_continue == false);
        ASSERT(a.recovery_action == RecoveryAction::None);
        ASSERT(a.host_notification == false);
        ASSERT(a.metrics_event == false);
        ASSERT(a.safe_cleanup_path == SafeCleanupPath::None);
    });
}

void error_attributes_field_modification(ContractTestRunner& runner) {
    runner.run("error_attributes_field_modification", []() {
        ErrorAttributes a{};
        a.severity = ErrorSeverity::Fatal;
        a.recoverable = true;
        a.retryable = true;
        a.requires_reopen_device = true;
        a.requires_rebuild_codec = true;
        a.requires_user_action = true;
        a.safe_to_continue = false;
        a.recovery_action = RecoveryAction::Retry;
        a.host_notification = true;
        a.metrics_event = true;
        a.safe_cleanup_path = SafeCleanupPath::GracefulDrain;

        ASSERT(a.severity == ErrorSeverity::Fatal);
        ASSERT(a.recoverable == true);
        ASSERT(a.retryable == true);
        ASSERT(a.requires_reopen_device == true);
        ASSERT(a.requires_rebuild_codec == true);
        ASSERT(a.requires_user_action == true);
        ASSERT(a.safe_to_continue == false);
        ASSERT(a.recovery_action == RecoveryAction::Retry);
        ASSERT(a.host_notification == true);
        ASSERT(a.metrics_event == true);
        ASSERT(a.safe_cleanup_path == SafeCleanupPath::GracefulDrain);
    });
}

void error_attributes_equality(ContractTestRunner& runner) {
    runner.run("error_attributes_equality", []() {
        ErrorAttributes a1{};
        ErrorAttributes a2{};
        ASSERT(a1 == a2);

        a1.severity = ErrorSeverity::Warning;
        ASSERT(!(a1 == a2));
        a2.severity = ErrorSeverity::Warning;
        ASSERT(a1 == a2);
    });
}

void error_attributes_has_all_required_fields(ContractTestRunner& runner) {
    runner.run("error_attributes_has_all_required_fields", []() {
        ErrorAttributes a{};
        // Verify all 11 fields exist by assigning to them
        a.severity = ErrorSeverity::Info;
        a.recoverable = true;
        a.retryable = true;
        a.requires_reopen_device = true;
        a.requires_rebuild_codec = true;
        a.requires_user_action = true;
        a.safe_to_continue = true;
        a.recovery_action = RecoveryAction::Drain;
        a.host_notification = true;
        a.metrics_event = true;
        a.safe_cleanup_path = SafeCleanupPath::ForceClose;
        ASSERT(a.severity == ErrorSeverity::Info);
    });
}

void error_attributes_copy(ContractTestRunner& runner) {
    runner.run("error_attributes_copy", []() {
        ErrorAttributes a1{};
        a1.severity = ErrorSeverity::Recoverable;
        a1.recoverable = true;
        a1.safe_cleanup_path = SafeCleanupPath::ImmediateFlush;

        ErrorAttributes a2 = a1;
        ASSERT(a1 == a2);
    });
}

} // namespace

void run_error_attributes_tests(ContractTestRunner& runner) {
    safe_cleanup_path_default_is_none(runner);
    safe_cleanup_path_graceful_drain(runner);
    safe_cleanup_path_immediate_flush(runner);
    safe_cleanup_path_force_close(runner);
    safe_cleanup_path_equality(runner);
    error_attributes_default_construction(runner);
    error_attributes_field_modification(runner);
    error_attributes_equality(runner);
    error_attributes_has_all_required_fields(runner);
    error_attributes_copy(runner);
}