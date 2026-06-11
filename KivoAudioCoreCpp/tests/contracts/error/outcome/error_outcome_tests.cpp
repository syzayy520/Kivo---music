#include "../../contract_tests_main.h"
#include "kivo/core/contract/error/outcome/error_outcome.hpp"

using namespace kivo::core::contract;

namespace {

void error_outcome_default_construction(ContractTestRunner& runner) {
    runner.run("error_outcome_default_construction", []() {
        ErrorOutcome o{};
        ASSERT(o.domain == ErrorDomain::Unknown);
        ASSERT(o.attributes.severity == ErrorSeverity::Unknown);
        ASSERT(o.attributes.recoverable == false);
        ASSERT(o.attributes.retryable == false);
        ASSERT(o.attributes.requires_reopen_device == false);
        ASSERT(o.attributes.requires_rebuild_codec == false);
        ASSERT(o.attributes.requires_user_action == false);
        ASSERT(o.attributes.safe_to_continue == false);
        ASSERT(o.attributes.recovery_action == RecoveryAction::None);
        ASSERT(o.attributes.host_notification == false);
        ASSERT(o.attributes.metrics_event == false);
        ASSERT(o.attributes.safe_cleanup_path == SafeCleanupPath::None);
        ASSERT(o.recommended_recovery == RecoveryAction::None);
    });
}

void error_outcome_field_modification(ContractTestRunner& runner) {
    runner.run("error_outcome_field_modification", []() {
        ErrorOutcome o{};
        o.domain = ErrorDomain::CodecError;
        o.attributes.severity = ErrorSeverity::Recoverable;
        o.attributes.recoverable = true;
        o.attributes.retryable = true;
        o.attributes.recovery_action = RecoveryAction::Retry;
        o.recommended_recovery = RecoveryAction::Retry;

        ASSERT(o.domain == ErrorDomain::CodecError);
        ASSERT(o.attributes.severity == ErrorSeverity::Recoverable);
        ASSERT(o.attributes.recoverable == true);
        ASSERT(o.attributes.retryable == true);
        ASSERT(o.attributes.recovery_action == RecoveryAction::Retry);
        ASSERT(o.recommended_recovery == RecoveryAction::Retry);
    });
}

void error_outcome_equality(ContractTestRunner& runner) {
    runner.run("error_outcome_equality", []() {
        ErrorOutcome o1{};
        ErrorOutcome o2{};
        ASSERT(o1 == o2);

        o1.domain = ErrorDomain::DeviceLost;
        ASSERT(!(o1 == o2));
        o2.domain = ErrorDomain::DeviceLost;
        ASSERT(o1 == o2);
    });
}

void error_outcome_copy(ContractTestRunner& runner) {
    runner.run("error_outcome_copy", []() {
        ErrorOutcome o1{};
        o1.domain = ErrorDomain::BufferUnderrun;
        o1.attributes.severity = ErrorSeverity::Warning;
        o1.attributes.recoverable = true;
        o1.recommended_recovery = RecoveryAction::Retry;

        ErrorOutcome o2 = o1;
        ASSERT(o1 == o2);
    });
}

void error_outcome_has_all_required_fields(ContractTestRunner& runner) {
    runner.run("error_outcome_has_all_required_fields", []() {
        ErrorOutcome o{};
        // Verify all 3 top-level fields exist by assigning to them
        o.domain = ErrorDomain::FormatChanged;
        o.attributes.severity = ErrorSeverity::Fatal;
        o.recommended_recovery = RecoveryAction::ReconfigureFormat;
        ASSERT(o.domain == ErrorDomain::FormatChanged);
    });
}

void error_outcome_with_device_lost(ContractTestRunner& runner) {
    runner.run("error_outcome_with_device_lost", []() {
        ErrorOutcome o{};
        o.domain = ErrorDomain::DeviceLost;
        o.attributes.severity = ErrorSeverity::Fatal;
        o.attributes.recoverable = false;
        o.attributes.requires_reopen_device = true;
        o.attributes.safe_to_continue = false;
        o.attributes.recovery_action = RecoveryAction::ReopenDevice;
        o.attributes.host_notification = true;
        o.attributes.safe_cleanup_path = SafeCleanupPath::ForceClose;
        o.recommended_recovery = RecoveryAction::ReopenDevice;

        ASSERT(o.domain == ErrorDomain::DeviceLost);
        ASSERT(o.attributes.severity == ErrorSeverity::Fatal);
        ASSERT(o.attributes.requires_reopen_device == true);
        ASSERT(o.recommended_recovery == RecoveryAction::ReopenDevice);
    });
}

void error_outcome_with_codec_starved(ContractTestRunner& runner) {
    runner.run("error_outcome_with_codec_starved", []() {
        ErrorOutcome o{};
        o.domain = ErrorDomain::CodecStarved;
        o.attributes.severity = ErrorSeverity::Recoverable;
        o.attributes.recoverable = true;
        o.attributes.retryable = true;
        o.attributes.safe_to_continue = true;
        o.attributes.recovery_action = RecoveryAction::Retry;
        o.attributes.safe_cleanup_path = SafeCleanupPath::GracefulDrain;
        o.recommended_recovery = RecoveryAction::Retry;

        ASSERT(o.domain == ErrorDomain::CodecStarved);
        ASSERT(o.attributes.recoverable == true);
        ASSERT(o.attributes.safe_to_continue == true);
        ASSERT(o.recommended_recovery == RecoveryAction::Retry);
    });
}

void error_outcome_with_network_interrupted(ContractTestRunner& runner) {
    runner.run("error_outcome_with_network_interrupted", []() {
        ErrorOutcome o{};
        o.domain = ErrorDomain::NetworkInterrupted;
        o.attributes.severity = ErrorSeverity::Warning;
        o.attributes.recoverable = true;
        o.attributes.retryable = true;
        o.attributes.safe_to_continue = true;
        o.attributes.recovery_action = RecoveryAction::Retry;
        o.attributes.metrics_event = true;
        o.recommended_recovery = RecoveryAction::Retry;

        ASSERT(o.domain == ErrorDomain::NetworkInterrupted);
        ASSERT(o.attributes.metrics_event == true);
    });
}

void error_outcome_with_permission_denied(ContractTestRunner& runner) {
    runner.run("error_outcome_with_permission_denied", []() {
        ErrorOutcome o{};
        o.domain = ErrorDomain::PermissionDenied;
        o.attributes.severity = ErrorSeverity::Fatal;
        o.attributes.recoverable = false;
        o.attributes.retryable = false;
        o.attributes.requires_user_action = true;
        o.attributes.safe_to_continue = false;
        o.attributes.recovery_action = RecoveryAction::ReportToUser;
        o.attributes.host_notification = true;
        o.recommended_recovery = RecoveryAction::ReportToUser;

        ASSERT(o.domain == ErrorDomain::PermissionDenied);
        ASSERT(o.attributes.requires_user_action == true);
        ASSERT(o.recommended_recovery == RecoveryAction::ReportToUser);
    });
}

void error_outcome_with_cache_invalid(ContractTestRunner& runner) {
    runner.run("error_outcome_with_cache_invalid", []() {
        ErrorOutcome o{};
        o.domain = ErrorDomain::CacheInvalid;
        o.attributes.severity = ErrorSeverity::Recoverable;
        o.attributes.recoverable = true;
        o.attributes.retryable = true;
        o.attributes.safe_to_continue = true;
        o.attributes.recovery_action = RecoveryAction::RebuildSource;
        o.recommended_recovery = RecoveryAction::RebuildSource;

        ASSERT(o.domain == ErrorDomain::CacheInvalid);
        ASSERT(o.recommended_recovery == RecoveryAction::RebuildSource);
    });
}

void error_outcome_with_internal_bug(ContractTestRunner& runner) {
    runner.run("error_outcome_with_internal_bug", []() {
        ErrorOutcome o{};
        o.domain = ErrorDomain::InternalBug;
        o.attributes.severity = ErrorSeverity::Fatal;
        o.attributes.recoverable = false;
        o.attributes.retryable = false;
        o.attributes.safe_to_continue = false;
        o.attributes.recovery_action = RecoveryAction::StopMedia;
        o.attributes.host_notification = true;
        o.attributes.metrics_event = true;
        o.attributes.safe_cleanup_path = SafeCleanupPath::ForceClose;
        o.recommended_recovery = RecoveryAction::StopMedia;

        ASSERT(o.domain == ErrorDomain::InternalBug);
        ASSERT(o.attributes.severity == ErrorSeverity::Fatal);
        ASSERT(o.recommended_recovery == RecoveryAction::StopMedia);
    });
}

} // namespace

void run_error_outcome_tests(ContractTestRunner& runner) {
    error_outcome_default_construction(runner);
    error_outcome_field_modification(runner);
    error_outcome_equality(runner);
    error_outcome_copy(runner);
    error_outcome_has_all_required_fields(runner);
    error_outcome_with_device_lost(runner);
    error_outcome_with_codec_starved(runner);
    error_outcome_with_network_interrupted(runner);
    error_outcome_with_permission_denied(runner);
    error_outcome_with_cache_invalid(runner);
    error_outcome_with_internal_bug(runner);
}