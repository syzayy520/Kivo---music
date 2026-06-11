#include "../../contract_tests_main.h"
#include "kivo/core/contract/source/container/container_identity.hpp"
#include "kivo/core/contract/source/container/container_capability.hpp"
#include "kivo/core/contract/source/container/container_boundary_contract.hpp"
#include "kivo/core/contract/source/container/container_failure.hpp"

namespace {

void container_identity_default_value_is_zero() {
    kivo::core::contract::ContainerIdentity id;
    ASSERT(id.value == 0);
}

void container_identity_equality() {
    kivo::core::contract::ContainerIdentity a{42};
    kivo::core::contract::ContainerIdentity b{42};
    ASSERT(a == b);
}

void container_identity_inequality() {
    kivo::core::contract::ContainerIdentity a{1};
    kivo::core::contract::ContainerIdentity b{2};
    ASSERT(!(a == b));
}

void container_capability_default_all_false() {
    kivo::core::contract::ContainerCapability cap;
    ASSERT(!cap.supports_chapters);
    ASSERT(!cap.supports_attachments);
    ASSERT(!cap.supports_multiple_streams);
}

void container_capability_field_modification() {
    kivo::core::contract::ContainerCapability cap;
    cap.supports_chapters = true;
    cap.supports_multiple_streams = true;
    ASSERT(cap.supports_chapters);
    ASSERT(!cap.supports_attachments);
    ASSERT(cap.supports_multiple_streams);
}

void container_capability_equality() {
    kivo::core::contract::ContainerCapability a;
    a.supports_chapters = true;
    kivo::core::contract::ContainerCapability b;
    b.supports_chapters = true;
    ASSERT(a == b);
}

void container_boundary_contract_default_construction() {
    kivo::core::contract::ContainerBoundaryContract contract;
    ASSERT(contract.id.value == 0);
    ASSERT(!contract.capability.supports_chapters);
    ASSERT(contract.ownership == kivo::core::contract::SourceOwnershipPolicy::NotSpecified);
}

void container_boundary_contract_field_modification() {
    kivo::core::contract::ContainerBoundaryContract contract;
    contract.id = kivo::core::contract::ContainerIdentity{77};
    contract.capability.supports_chapters = true;
    contract.ownership = kivo::core::contract::SourceOwnershipPolicy::Shared;
    ASSERT(contract.id.value == 77);
    ASSERT(contract.capability.supports_chapters);
    ASSERT(contract.ownership == kivo::core::contract::SourceOwnershipPolicy::Shared);
}

void container_boundary_contract_equality() {
    kivo::core::contract::ContainerBoundaryContract a;
    a.id = kivo::core::contract::ContainerIdentity{5};
    kivo::core::contract::ContainerBoundaryContract b;
    b.id = kivo::core::contract::ContainerIdentity{5};
    ASSERT(a == b);
}

void container_failure_default_is_not_specified() {
    kivo::core::contract::ContainerFailure f{};
    ASSERT(f == kivo::core::contract::ContainerFailure::NotSpecified);
}

void container_failure_corrupted_header() {
    auto f = kivo::core::contract::ContainerFailure::CorruptedHeader;
    ASSERT(f != kivo::core::contract::ContainerFailure::NotSpecified);
}

void container_failure_equality() {
    auto a = kivo::core::contract::ContainerFailure::UnsupportedFormat;
    auto b = kivo::core::contract::ContainerFailure::UnsupportedFormat;
    ASSERT(a == b);
}

} // namespace

void run_source_container_tests(ContractTestRunner& runner) {
    runner.run("container_identity_default_value_is_zero", container_identity_default_value_is_zero);
    runner.run("container_identity_equality", container_identity_equality);
    runner.run("container_identity_inequality", container_identity_inequality);
    runner.run("container_capability_default_all_false", container_capability_default_all_false);
    runner.run("container_capability_field_modification", container_capability_field_modification);
    runner.run("container_capability_equality", container_capability_equality);
    runner.run("container_boundary_contract_default_construction", container_boundary_contract_default_construction);
    runner.run("container_boundary_contract_field_modification", container_boundary_contract_field_modification);
    runner.run("container_boundary_contract_equality", container_boundary_contract_equality);
    runner.run("container_failure_default_is_not_specified", container_failure_default_is_not_specified);
    runner.run("container_failure_corrupted_header", container_failure_corrupted_header);
    runner.run("container_failure_equality", container_failure_equality);
}
