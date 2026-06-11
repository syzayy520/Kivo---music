#include "../../contract_tests_main.h"
#include "kivo/core/contract/source/boundary/source_boundary_contract.hpp"
#include "kivo/core/contract/source/boundary/source_boundary_mode.hpp"

namespace {

void source_boundary_contract_default_construction() {
    kivo::core::contract::SourceBoundaryContract contract;
    ASSERT(contract.reader.id.value == 0);
    ASSERT(contract.container.id.value == 0);
    ASSERT(contract.readiness == kivo::core::contract::SourceReadiness::NotSpecified);
    ASSERT(contract.fault_policy == kivo::core::contract::SourceFaultPolicy::NotSpecified);
}

void source_boundary_contract_field_modification() {
    kivo::core::contract::SourceBoundaryContract contract;
    contract.reader.id = kivo::core::contract::ReaderIdentity{10};
    contract.reader.capability.can_seek = true;
    contract.container.id = kivo::core::contract::ContainerIdentity{20};
    contract.container.capability.supports_chapters = true;
    contract.readiness = kivo::core::contract::SourceReadiness::Ready;
    contract.fault_policy = kivo::core::contract::SourceFaultPolicy::RetryOnce;
    ASSERT(contract.reader.id.value == 10);
    ASSERT(contract.reader.capability.can_seek);
    ASSERT(contract.container.id.value == 20);
    ASSERT(contract.container.capability.supports_chapters);
    ASSERT(contract.readiness == kivo::core::contract::SourceReadiness::Ready);
    ASSERT(contract.fault_policy == kivo::core::contract::SourceFaultPolicy::RetryOnce);
}

void source_boundary_contract_equality() {
    kivo::core::contract::SourceBoundaryContract a;
    a.readiness = kivo::core::contract::SourceReadiness::Ready;
    kivo::core::contract::SourceBoundaryContract b;
    b.readiness = kivo::core::contract::SourceReadiness::Ready;
    ASSERT(a == b);
}

void source_boundary_contract_inequality() {
    kivo::core::contract::SourceBoundaryContract a;
    a.readiness = kivo::core::contract::SourceReadiness::Ready;
    kivo::core::contract::SourceBoundaryContract b;
    b.readiness = kivo::core::contract::SourceReadiness::Failed;
    ASSERT(!(a == b));
}

void source_boundary_mode_default_is_not_specified() {
    kivo::core::contract::SourceBoundaryMode mode{};
    ASSERT(mode == kivo::core::contract::SourceBoundaryMode::NotSpecified);
}

void source_boundary_mode_synchronous() {
    auto mode = kivo::core::contract::SourceBoundaryMode::Synchronous;
    ASSERT(mode != kivo::core::contract::SourceBoundaryMode::NotSpecified);
}

void source_boundary_mode_asynchronous() {
    auto mode = kivo::core::contract::SourceBoundaryMode::Asynchronous;
    ASSERT(mode != kivo::core::contract::SourceBoundaryMode::Synchronous);
}

void source_boundary_mode_equality() {
    auto a = kivo::core::contract::SourceBoundaryMode::Streaming;
    auto b = kivo::core::contract::SourceBoundaryMode::Streaming;
    ASSERT(a == b);
}

} // namespace

void run_source_boundary_tests(ContractTestRunner& runner) {
    runner.run("source_boundary_contract_default_construction", source_boundary_contract_default_construction);
    runner.run("source_boundary_contract_field_modification", source_boundary_contract_field_modification);
    runner.run("source_boundary_contract_equality", source_boundary_contract_equality);
    runner.run("source_boundary_contract_inequality", source_boundary_contract_inequality);
    runner.run("source_boundary_mode_default_is_not_specified", source_boundary_mode_default_is_not_specified);
    runner.run("source_boundary_mode_synchronous", source_boundary_mode_synchronous);
    runner.run("source_boundary_mode_asynchronous", source_boundary_mode_asynchronous);
    runner.run("source_boundary_mode_equality", source_boundary_mode_equality);
}
