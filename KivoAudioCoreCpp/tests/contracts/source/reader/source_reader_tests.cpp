#include "../../contract_tests_main.h"
#include "kivo/core/contract/source/reader/reader_identity.hpp"
#include "kivo/core/contract/source/reader/reader_capability.hpp"
#include "kivo/core/contract/source/reader/reader_boundary_contract.hpp"
#include "kivo/core/contract/source/reader/reader_failure.hpp"

namespace {

void reader_identity_default_value_is_zero() {
    kivo::core::contract::ReaderIdentity id;
    ASSERT(id.value == 0);
}

void reader_identity_equality() {
    kivo::core::contract::ReaderIdentity a{42};
    kivo::core::contract::ReaderIdentity b{42};
    ASSERT(a == b);
}

void reader_identity_inequality() {
    kivo::core::contract::ReaderIdentity a{1};
    kivo::core::contract::ReaderIdentity b{2};
    ASSERT(!(a == b));
}

void reader_capability_default_all_false() {
    kivo::core::contract::ReaderCapability cap;
    ASSERT(!cap.can_seek);
    ASSERT(!cap.can_pause);
    ASSERT(!cap.supports_metadata);
}

void reader_capability_field_modification() {
    kivo::core::contract::ReaderCapability cap;
    cap.can_seek = true;
    cap.supports_metadata = true;
    ASSERT(cap.can_seek);
    ASSERT(!cap.can_pause);
    ASSERT(cap.supports_metadata);
}

void reader_capability_equality() {
    kivo::core::contract::ReaderCapability a;
    a.can_seek = true;
    kivo::core::contract::ReaderCapability b;
    b.can_seek = true;
    ASSERT(a == b);
}

void reader_boundary_contract_default_construction() {
    kivo::core::contract::ReaderBoundaryContract contract;
    ASSERT(contract.id.value == 0);
    ASSERT(!contract.capability.can_seek);
    ASSERT(contract.ownership == kivo::core::contract::SourceOwnershipPolicy::NotSpecified);
}

void reader_boundary_contract_field_modification() {
    kivo::core::contract::ReaderBoundaryContract contract;
    contract.id = kivo::core::contract::ReaderIdentity{99};
    contract.capability.can_seek = true;
    contract.ownership = kivo::core::contract::SourceOwnershipPolicy::Owned;
    ASSERT(contract.id.value == 99);
    ASSERT(contract.capability.can_seek);
    ASSERT(contract.ownership == kivo::core::contract::SourceOwnershipPolicy::Owned);
}

void reader_boundary_contract_equality() {
    kivo::core::contract::ReaderBoundaryContract a;
    a.id = kivo::core::contract::ReaderIdentity{10};
    kivo::core::contract::ReaderBoundaryContract b;
    b.id = kivo::core::contract::ReaderIdentity{10};
    ASSERT(a == b);
}

void reader_failure_default_is_not_specified() {
    kivo::core::contract::ReaderFailure f{};
    ASSERT(f == kivo::core::contract::ReaderFailure::NotSpecified);
}

void reader_failure_io_error() {
    auto f = kivo::core::contract::ReaderFailure::IoError;
    ASSERT(f != kivo::core::contract::ReaderFailure::NotSpecified);
}

void reader_failure_equality() {
    auto a = kivo::core::contract::ReaderFailure::FormatError;
    auto b = kivo::core::contract::ReaderFailure::FormatError;
    ASSERT(a == b);
}

} // namespace

void run_source_reader_tests(ContractTestRunner& runner) {
    runner.run("reader_identity_default_value_is_zero", reader_identity_default_value_is_zero);
    runner.run("reader_identity_equality", reader_identity_equality);
    runner.run("reader_identity_inequality", reader_identity_inequality);
    runner.run("reader_capability_default_all_false", reader_capability_default_all_false);
    runner.run("reader_capability_field_modification", reader_capability_field_modification);
    runner.run("reader_capability_equality", reader_capability_equality);
    runner.run("reader_boundary_contract_default_construction", reader_boundary_contract_default_construction);
    runner.run("reader_boundary_contract_field_modification", reader_boundary_contract_field_modification);
    runner.run("reader_boundary_contract_equality", reader_boundary_contract_equality);
    runner.run("reader_failure_default_is_not_specified", reader_failure_default_is_not_specified);
    runner.run("reader_failure_io_error", reader_failure_io_error);
    runner.run("reader_failure_equality", reader_failure_equality);
}
