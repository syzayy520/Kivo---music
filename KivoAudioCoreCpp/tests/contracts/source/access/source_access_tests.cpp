#include "../../contract_tests_main.h"
#include "kivo/core/contract/source/access/source_access_type.hpp"
#include "kivo/core/contract/source/access/source_readiness.hpp"
#include "kivo/core/contract/source/access/source_ownership_policy.hpp"
#include "kivo/core/contract/source/access/source_fault_policy.hpp"

namespace {

void source_type_default_is_not_specified() {
    kivo::core::contract::SourceType t{};
    ASSERT(t == kivo::core::contract::SourceType::NotSpecified);
}

void source_type_file() {
    auto t = kivo::core::contract::SourceType::File;
    ASSERT(t != kivo::core::contract::SourceType::NotSpecified);
}

void source_type_stream() {
    auto t = kivo::core::contract::SourceType::Stream;
    ASSERT(t != kivo::core::contract::SourceType::File);
}

void source_readiness_default_is_not_specified() {
    kivo::core::contract::SourceReadiness r{};
    ASSERT(r == kivo::core::contract::SourceReadiness::NotSpecified);
}

void source_readiness_ready() {
    auto r = kivo::core::contract::SourceReadiness::Ready;
    ASSERT(r != kivo::core::contract::SourceReadiness::NotSpecified);
}

void source_readiness_failed() {
    auto r = kivo::core::contract::SourceReadiness::Failed;
    ASSERT(r != kivo::core::contract::SourceReadiness::Ready);
}

void source_ownership_default_is_not_specified() {
    kivo::core::contract::SourceOwnershipPolicy o{};
    ASSERT(o == kivo::core::contract::SourceOwnershipPolicy::NotSpecified);
}

void source_ownership_owned() {
    auto o = kivo::core::contract::SourceOwnershipPolicy::Owned;
    ASSERT(o != kivo::core::contract::SourceOwnershipPolicy::NotSpecified);
}

void source_ownership_shared() {
    auto o = kivo::core::contract::SourceOwnershipPolicy::Shared;
    ASSERT(o != kivo::core::contract::SourceOwnershipPolicy::Owned);
}

void source_fault_default_is_not_specified() {
    kivo::core::contract::SourceFaultPolicy f{};
    ASSERT(f == kivo::core::contract::SourceFaultPolicy::NotSpecified);
}

void source_fault_fail_immediate() {
    auto f = kivo::core::contract::SourceFaultPolicy::FailImmediate;
    ASSERT(f != kivo::core::contract::SourceFaultPolicy::NotSpecified);
}

void source_fault_retry_once() {
    auto f = kivo::core::contract::SourceFaultPolicy::RetryOnce;
    ASSERT(f != kivo::core::contract::SourceFaultPolicy::FailImmediate);
}

void source_fault_skip_and_continue() {
    auto f = kivo::core::contract::SourceFaultPolicy::SkipAndContinue;
    ASSERT(f != kivo::core::contract::SourceFaultPolicy::ReportAndContinue);
}

void source_fault_equality() {
    auto a = kivo::core::contract::SourceFaultPolicy::RetryWithBackoff;
    auto b = kivo::core::contract::SourceFaultPolicy::RetryWithBackoff;
    ASSERT(a == b);
}

} // namespace

void run_source_access_tests(ContractTestRunner& runner) {
    runner.run("source_type_default_is_not_specified", source_type_default_is_not_specified);
    runner.run("source_type_file", source_type_file);
    runner.run("source_type_stream", source_type_stream);
    runner.run("source_readiness_default_is_not_specified", source_readiness_default_is_not_specified);
    runner.run("source_readiness_ready", source_readiness_ready);
    runner.run("source_readiness_failed", source_readiness_failed);
    runner.run("source_ownership_default_is_not_specified", source_ownership_default_is_not_specified);
    runner.run("source_ownership_owned", source_ownership_owned);
    runner.run("source_ownership_shared", source_ownership_shared);
    runner.run("source_fault_default_is_not_specified", source_fault_default_is_not_specified);
    runner.run("source_fault_fail_immediate", source_fault_fail_immediate);
    runner.run("source_fault_retry_once", source_fault_retry_once);
    runner.run("source_fault_skip_and_continue", source_fault_skip_and_continue);
    runner.run("source_fault_equality", source_fault_equality);
}
