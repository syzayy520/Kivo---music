#include "../../contract_tests_main.h"
#include "kivo/core/contract/realtime/ownership/ownership_transfer_rule.hpp"
#include "kivo/core/contract/realtime/ownership/ownership_visibility_rule.hpp"

using namespace kivo::core::contract;

void run_realtime_ownership_contract_tests(ContractTestRunner& runner) {
    runner.run("ownership_transfer_not_specified", []() {
        ASSERT(OwnershipTransfer{OwnershipTransfer::NotSpecified} == OwnershipTransfer::NotSpecified);
    });
    
    runner.run("ownership_transfer_explicit_required", []() {
        ASSERT(OwnershipTransfer{OwnershipTransfer::ExplicitTransferRequired} == OwnershipTransfer::ExplicitTransferRequired);
    });
    
    runner.run("ownership_transfer_implicit_allowed", []() {
        ASSERT(OwnershipTransfer{OwnershipTransfer::ImplicitTransferAllowed} == OwnershipTransfer::ImplicitTransferAllowed);
    });
    
    runner.run("ownership_transfer_forbidden", []() {
        ASSERT(OwnershipTransfer{OwnershipTransfer::TransferForbidden} == OwnershipTransfer::TransferForbidden);
    });
    
    runner.run("ownership_transfer_enum_values", []() {
        ASSERT(static_cast<uint8_t>(OwnershipTransfer::NotSpecified) == 0);
        ASSERT(static_cast<uint8_t>(OwnershipTransfer::ExplicitTransferRequired) == 1);
        ASSERT(static_cast<uint8_t>(OwnershipTransfer::ImplicitTransferAllowed) == 2);
        ASSERT(static_cast<uint8_t>(OwnershipTransfer::TransferForbidden) == 3);
    });
    
    runner.run("ownership_visibility_not_specified", []() {
        ASSERT(OwnershipVisibility{OwnershipVisibility::NotSpecified} == OwnershipVisibility::NotSpecified);
    });
    
    runner.run("ownership_visibility_must_be_provable", []() {
        ASSERT(OwnershipVisibility{OwnershipVisibility::MustBeProvable} == OwnershipVisibility::MustBeProvable);
    });
    
    runner.run("ownership_visibility_best_effort", []() {
        ASSERT(OwnershipVisibility{OwnershipVisibility::BestEffort} == OwnershipVisibility::BestEffort);
    });
    
    runner.run("ownership_visibility_no_requirement", []() {
        ASSERT(OwnershipVisibility{OwnershipVisibility::NoRequirement} == OwnershipVisibility::NoRequirement);
    });
    
    runner.run("ownership_visibility_enum_values", []() {
        ASSERT(static_cast<uint8_t>(OwnershipVisibility::NotSpecified) == 0);
        ASSERT(static_cast<uint8_t>(OwnershipVisibility::MustBeProvable) == 1);
        ASSERT(static_cast<uint8_t>(OwnershipVisibility::BestEffort) == 2);
        ASSERT(static_cast<uint8_t>(OwnershipVisibility::NoRequirement) == 3);
    });
}