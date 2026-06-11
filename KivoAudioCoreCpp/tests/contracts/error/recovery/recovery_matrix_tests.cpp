#include "../../contract_tests_main.h"
#include "kivo/core/contract/error/recovery/recovery_matrix.hpp"

using namespace kivo::core::contract;

namespace {

void recovery_matrix_default_construction(ContractTestRunner& runner) {
    runner.run("recovery_matrix_default_construction", []() {
        RecoveryMatrix m{};
        ASSERT(m.domain == ErrorDomain::Unknown);
        ASSERT(m.primary_action == RecoveryAction::None);
        ASSERT(m.fallback_action == RecoveryAction::None);
    });
}

void recovery_matrix_field_modification(ContractTestRunner& runner) {
    runner.run("recovery_matrix_field_modification", []() {
        RecoveryMatrix m{};
        m.domain = ErrorDomain::CodecError;
        m.primary_action = RecoveryAction::Retry;
        m.fallback_action = RecoveryAction::SkipFrame;

        ASSERT(m.domain == ErrorDomain::CodecError);
        ASSERT(m.primary_action == RecoveryAction::Retry);
        ASSERT(m.fallback_action == RecoveryAction::SkipFrame);
    });
}

void recovery_matrix_equality(ContractTestRunner& runner) {
    runner.run("recovery_matrix_equality", []() {
        RecoveryMatrix m1{};
        RecoveryMatrix m2{};
        ASSERT(m1 == m2);

        m1.domain = ErrorDomain::DeviceLost;
        ASSERT(!(m1 == m2));
        m2.domain = ErrorDomain::DeviceLost;
        ASSERT(m1 == m2);
    });
}

void recovery_matrix_copy(ContractTestRunner& runner) {
    runner.run("recovery_matrix_copy", []() {
        RecoveryMatrix m1{};
        m1.domain = ErrorDomain::BufferUnderrun;
        m1.primary_action = RecoveryAction::Retry;
        m1.fallback_action = RecoveryAction::ReopenDevice;

        RecoveryMatrix m2 = m1;
        ASSERT(m1 == m2);
    });
}

void recovery_matrix_has_all_required_fields(ContractTestRunner& runner) {
    runner.run("recovery_matrix_has_all_required_fields", []() {
        RecoveryMatrix m{};
        // Verify all 3 fields exist by assigning to them
        m.domain = ErrorDomain::FormatChanged;
        m.primary_action = RecoveryAction::ReconfigureFormat;
        m.fallback_action = RecoveryAction::RebuildCodec;
        ASSERT(m.domain == ErrorDomain::FormatChanged);
    });
}

void recovery_matrix_primary_only(ContractTestRunner& runner) {
    runner.run("recovery_matrix_primary_only", []() {
        RecoveryMatrix m{};
        m.domain = ErrorDomain::NetworkInterrupted;
        m.primary_action = RecoveryAction::Retry;
        ASSERT(m.fallback_action == RecoveryAction::None);
    });
}

void recovery_matrix_with_fallback(ContractTestRunner& runner) {
    runner.run("recovery_matrix_with_fallback", []() {
        RecoveryMatrix m{};
        m.domain = ErrorDomain::DeviceError;
        m.primary_action = RecoveryAction::ReopenDevice;
        m.fallback_action = RecoveryAction::StopMedia;
        ASSERT(m.primary_action != m.fallback_action);
    });
}

void recovery_matrix_all_error_domains(ContractTestRunner& runner) {
    runner.run("recovery_matrix_all_error_domains", []() {
        // Verify RecoveryMatrix can be used with all ErrorDomain values
        RecoveryMatrix m{};
        m.domain = ErrorDomain::UserError;
        ASSERT(m.domain == ErrorDomain::UserError);
        m.domain = ErrorDomain::MediaError;
        ASSERT(m.domain == ErrorDomain::MediaError);
        m.domain = ErrorDomain::CodecError;
        ASSERT(m.domain == ErrorDomain::CodecError);
        m.domain = ErrorDomain::FormatError;
        ASSERT(m.domain == ErrorDomain::FormatError);
        m.domain = ErrorDomain::DeviceError;
        ASSERT(m.domain == ErrorDomain::DeviceError);
        m.domain = ErrorDomain::BackendError;
        ASSERT(m.domain == ErrorDomain::BackendError);
        m.domain = ErrorDomain::HostError;
        ASSERT(m.domain == ErrorDomain::HostError);
        m.domain = ErrorDomain::InternalBug;
        ASSERT(m.domain == ErrorDomain::InternalBug);
        m.domain = ErrorDomain::DeviceLost;
        ASSERT(m.domain == ErrorDomain::DeviceLost);
        m.domain = ErrorDomain::FormatChanged;
        ASSERT(m.domain == ErrorDomain::FormatChanged);
        m.domain = ErrorDomain::CodecStarved;
        ASSERT(m.domain == ErrorDomain::CodecStarved);
        m.domain = ErrorDomain::BufferUnderrun;
        ASSERT(m.domain == ErrorDomain::BufferUnderrun);
        m.domain = ErrorDomain::BufferOverrun;
        ASSERT(m.domain == ErrorDomain::BufferOverrun);
        m.domain = ErrorDomain::UnsupportedFormat;
        ASSERT(m.domain == ErrorDomain::UnsupportedFormat);
        m.domain = ErrorDomain::SourceUnavailable;
        ASSERT(m.domain == ErrorDomain::SourceUnavailable);
        m.domain = ErrorDomain::NetworkInterrupted;
        ASSERT(m.domain == ErrorDomain::NetworkInterrupted);
        m.domain = ErrorDomain::PermissionDenied;
        ASSERT(m.domain == ErrorDomain::PermissionDenied);
        m.domain = ErrorDomain::CacheInvalid;
        ASSERT(m.domain == ErrorDomain::CacheInvalid);
    });
}

} // namespace

void run_recovery_matrix_tests(ContractTestRunner& runner) {
    recovery_matrix_default_construction(runner);
    recovery_matrix_field_modification(runner);
    recovery_matrix_equality(runner);
    recovery_matrix_copy(runner);
    recovery_matrix_has_all_required_fields(runner);
    recovery_matrix_primary_only(runner);
    recovery_matrix_with_fallback(runner);
    recovery_matrix_all_error_domains(runner);
}