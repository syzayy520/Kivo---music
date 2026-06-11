#include "../../contract_tests_main.h"
#include "kivo/core/contract/source/outcome/source_outcome.hpp"
#include "kivo/core/contract/source/outcome/source_outcome_reason.hpp"
#include "kivo/core/contract/source/outcome/source_recovery_policy.hpp"

namespace {

void source_outcome_reason_default_is_not_specified() {
    kivo::core::contract::SourceOutcomeReason r{};
    ASSERT(r == kivo::core::contract::SourceOutcomeReason::NotSpecified);
}

void source_outcome_reason_success() {
    auto r = kivo::core::contract::SourceOutcomeReason::Success;
    ASSERT(r != kivo::core::contract::SourceOutcomeReason::NotSpecified);
}

void source_outcome_reason_io_failure() {
    auto r = kivo::core::contract::SourceOutcomeReason::IoFailure;
    ASSERT(r != kivo::core::contract::SourceOutcomeReason::Success);
}

void source_outcome_reason_equality() {
    auto a = kivo::core::contract::SourceOutcomeReason::FormatFailure;
    auto b = kivo::core::contract::SourceOutcomeReason::FormatFailure;
    ASSERT(a == b);
}

void source_recovery_policy_default_is_not_specified() {
    kivo::core::contract::SourceRecoveryPolicy p{};
    ASSERT(p == kivo::core::contract::SourceRecoveryPolicy::NotSpecified);
}

void source_recovery_policy_no_recovery() {
    auto p = kivo::core::contract::SourceRecoveryPolicy::NoRecovery;
    ASSERT(p != kivo::core::contract::SourceRecoveryPolicy::NotSpecified);
}

void source_recovery_policy_retry_same() {
    auto p = kivo::core::contract::SourceRecoveryPolicy::RetrySameSource;
    ASSERT(p != kivo::core::contract::SourceRecoveryPolicy::RetryAlternateSource);
}

void source_recovery_policy_equality() {
    auto a = kivo::core::contract::SourceRecoveryPolicy::SkipToNext;
    auto b = kivo::core::contract::SourceRecoveryPolicy::SkipToNext;
    ASSERT(a == b);
}

void source_outcome_default_construction() {
    kivo::core::contract::SourceOutcome outcome;
    ASSERT(outcome.reason == kivo::core::contract::SourceOutcomeReason::NotSpecified);
    ASSERT(outcome.recovery == kivo::core::contract::SourceRecoveryPolicy::NotSpecified);
}

void source_outcome_field_modification() {
    kivo::core::contract::SourceOutcome outcome;
    outcome.reason = kivo::core::contract::SourceOutcomeReason::Success;
    outcome.recovery = kivo::core::contract::SourceRecoveryPolicy::NoRecovery;
    ASSERT(outcome.reason == kivo::core::contract::SourceOutcomeReason::Success);
    ASSERT(outcome.recovery == kivo::core::contract::SourceRecoveryPolicy::NoRecovery);
}

void source_outcome_equality() {
    kivo::core::contract::SourceOutcome a;
    a.reason = kivo::core::contract::SourceOutcomeReason::IoFailure;
    a.recovery = kivo::core::contract::SourceRecoveryPolicy::RetrySameSource;
    kivo::core::contract::SourceOutcome b;
    b.reason = kivo::core::contract::SourceOutcomeReason::IoFailure;
    b.recovery = kivo::core::contract::SourceRecoveryPolicy::RetrySameSource;
    ASSERT(a == b);
}

} // namespace

void run_source_outcome_tests(ContractTestRunner& runner) {
    runner.run("source_outcome_reason_default_is_not_specified", source_outcome_reason_default_is_not_specified);
    runner.run("source_outcome_reason_success", source_outcome_reason_success);
    runner.run("source_outcome_reason_io_failure", source_outcome_reason_io_failure);
    runner.run("source_outcome_reason_equality", source_outcome_reason_equality);
    runner.run("source_recovery_policy_default_is_not_specified", source_recovery_policy_default_is_not_specified);
    runner.run("source_recovery_policy_no_recovery", source_recovery_policy_no_recovery);
    runner.run("source_recovery_policy_retry_same", source_recovery_policy_retry_same);
    runner.run("source_recovery_policy_equality", source_recovery_policy_equality);
    runner.run("source_outcome_default_construction", source_outcome_default_construction);
    runner.run("source_outcome_field_modification", source_outcome_field_modification);
    runner.run("source_outcome_equality", source_outcome_equality);
}
