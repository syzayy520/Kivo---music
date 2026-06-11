#include "../../contract_tests_main.h"
#include "kivo/core/contract/policy/privacy/privacy_boundary_rule.hpp"
#include "kivo/core/contract/policy/privacy/path_exposure_policy.hpp"
#include "kivo/core/contract/policy/privacy/source_identity_sanitization.hpp"
#include "kivo/core/contract/policy/privacy/observation_control_policy.hpp"

using namespace kivo::core::contract::policy;

namespace {

void privacy_boundary_rule_default_construction(ContractTestRunner& runner) {
    runner.run("privacy_boundary_rule_default_construction", []() {
        PrivacyBoundaryRule rule{};
        ASSERT(rule.expose_local_paths == 0);
        ASSERT(rule.expose_user_names == 0);
        ASSERT(rule.expose_file_metadata == 0);
    });
}

void privacy_boundary_rule_field_modification(ContractTestRunner& runner) {
    runner.run("privacy_boundary_rule_field_modification", []() {
        PrivacyBoundaryRule rule{};
        rule.expose_local_paths = 1;
        rule.expose_user_names = 2;
        rule.expose_file_metadata = 3;
        ASSERT(rule.expose_local_paths == 1);
        ASSERT(rule.expose_user_names == 2);
        ASSERT(rule.expose_file_metadata == 3);
    });
}

void privacy_boundary_rule_equality(ContractTestRunner& runner) {
    runner.run("privacy_boundary_rule_equality", []() {
        PrivacyBoundaryRule a{};
        PrivacyBoundaryRule b{};
        ASSERT(a == b);
        a.expose_local_paths = 1;
        ASSERT(!(a == b));
        b.expose_local_paths = 1;
        ASSERT(a == b);
    });
}

void path_exposure_policy_default_construction(ContractTestRunner& runner) {
    runner.run("path_exposure_policy_default_construction", []() {
        PathExposurePolicy policy{};
        ASSERT(policy.allow_absolute_paths == 0);
        ASSERT(policy.allow_home_relative == 0);
        ASSERT(policy.sanitize_separators == 0);
    });
}

void path_exposure_policy_field_modification(ContractTestRunner& runner) {
    runner.run("path_exposure_policy_field_modification", []() {
        PathExposurePolicy policy{};
        policy.allow_absolute_paths = 1;
        policy.allow_home_relative = 2;
        policy.sanitize_separators = 3;
        ASSERT(policy.allow_absolute_paths == 1);
        ASSERT(policy.allow_home_relative == 2);
        ASSERT(policy.sanitize_separators == 3);
    });
}

void path_exposure_policy_equality(ContractTestRunner& runner) {
    runner.run("path_exposure_policy_equality", []() {
        PathExposurePolicy a{};
        PathExposurePolicy b{};
        ASSERT(a == b);
        a.allow_absolute_paths = 1;
        ASSERT(!(a == b));
        b.allow_absolute_paths = 1;
        ASSERT(a == b);
    });
}

void source_identity_sanitization_default_construction(ContractTestRunner& runner) {
    runner.run("source_identity_sanitization_default_construction", []() {
        SourceIdentitySanitization s{};
        ASSERT(s.strip_username == 0);
        ASSERT(s.strip_hostname == 0);
        ASSERT(s.strip_device_serial == 0);
    });
}

void source_identity_sanitization_field_modification(ContractTestRunner& runner) {
    runner.run("source_identity_sanitization_field_modification", []() {
        SourceIdentitySanitization s{};
        s.strip_username = 1;
        s.strip_hostname = 2;
        s.strip_device_serial = 3;
        ASSERT(s.strip_username == 1);
        ASSERT(s.strip_hostname == 2);
        ASSERT(s.strip_device_serial == 3);
    });
}

void source_identity_sanitization_equality(ContractTestRunner& runner) {
    runner.run("source_identity_sanitization_equality", []() {
        SourceIdentitySanitization a{};
        SourceIdentitySanitization b{};
        ASSERT(a == b);
        a.strip_username = 1;
        ASSERT(!(a == b));
        b.strip_username = 1;
        ASSERT(a == b);
    });
}

void observation_control_policy_default_construction(ContractTestRunner& runner) {
    runner.run("observation_control_policy_default_construction", []() {
        ObservationControlPolicy policy{};
        ASSERT(policy.allow_metric_export == 0);
        ASSERT(policy.allow_trace_export == 0);
        ASSERT(policy.allow_diagnostic_export == 0);
    });
}

void observation_control_policy_field_modification(ContractTestRunner& runner) {
    runner.run("observation_control_policy_field_modification", []() {
        ObservationControlPolicy policy{};
        policy.allow_metric_export = 1;
        policy.allow_trace_export = 2;
        policy.allow_diagnostic_export = 3;
        ASSERT(policy.allow_metric_export == 1);
        ASSERT(policy.allow_trace_export == 2);
        ASSERT(policy.allow_diagnostic_export == 3);
    });
}

void observation_control_policy_equality(ContractTestRunner& runner) {
    runner.run("observation_control_policy_equality", []() {
        ObservationControlPolicy a{};
        ObservationControlPolicy b{};
        ASSERT(a == b);
        a.allow_metric_export = 1;
        ASSERT(!(a == b));
        b.allow_metric_export = 1;
        ASSERT(a == b);
    });
}

} // anonymous namespace

void run_privacy_policy_tests(ContractTestRunner& runner) {
    privacy_boundary_rule_default_construction(runner);
    privacy_boundary_rule_field_modification(runner);
    privacy_boundary_rule_equality(runner);
    path_exposure_policy_default_construction(runner);
    path_exposure_policy_field_modification(runner);
    path_exposure_policy_equality(runner);
    source_identity_sanitization_default_construction(runner);
    source_identity_sanitization_field_modification(runner);
    source_identity_sanitization_equality(runner);
    observation_control_policy_default_construction(runner);
    observation_control_policy_field_modification(runner);
    observation_control_policy_equality(runner);
}
