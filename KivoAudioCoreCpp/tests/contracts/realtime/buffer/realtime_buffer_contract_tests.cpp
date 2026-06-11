#include "../../contract_tests_main.h"
#include "kivo/core/contract/realtime/buffer/preallocated_buffer_rule.hpp"
#include "kivo/core/contract/realtime/buffer/buffer_ownership_rule.hpp"
#include "kivo/core/contract/realtime/buffer/buffer_lifetime_rule.hpp"

using namespace kivo::core::contract;

void run_realtime_buffer_contract_tests(ContractTestRunner& runner) {
    runner.run("preallocation_requirement_not_specified", []() {
        ASSERT(PreallocationRequirement{PreallocationRequirement::NotSpecified} == PreallocationRequirement::NotSpecified);
    });
    
    runner.run("preallocation_requirement_required", []() {
        ASSERT(PreallocationRequirement{PreallocationRequirement::Required} == PreallocationRequirement::Required);
    });
    
    runner.run("preallocation_requirement_recommended", []() {
        ASSERT(PreallocationRequirement{PreallocationRequirement::Recommended} == PreallocationRequirement::Recommended);
    });
    
    runner.run("preallocation_requirement_not_required", []() {
        ASSERT(PreallocationRequirement{PreallocationRequirement::NotRequired} == PreallocationRequirement::NotRequired);
    });
    
    runner.run("preallocation_requirement_enum_values", []() {
        ASSERT(static_cast<uint8_t>(PreallocationRequirement::NotSpecified) == 0);
        ASSERT(static_cast<uint8_t>(PreallocationRequirement::Required) == 1);
        ASSERT(static_cast<uint8_t>(PreallocationRequirement::Recommended) == 2);
        ASSERT(static_cast<uint8_t>(PreallocationRequirement::NotRequired) == 3);
    });
    
    runner.run("buffer_ownership_not_specified", []() {
        ASSERT(BufferOwnership{BufferOwnership::NotSpecified} == BufferOwnership::NotSpecified);
    });
    
    runner.run("buffer_ownership_explicit_required", []() {
        ASSERT(BufferOwnership{BufferOwnership::ExplicitRequired} == BufferOwnership::ExplicitRequired);
    });
    
    runner.run("buffer_ownership_implicit_allowed", []() {
        ASSERT(BufferOwnership{BufferOwnership::ImplicitAllowed} == BufferOwnership::ImplicitAllowed);
    });
    
    runner.run("buffer_ownership_enum_values", []() {
        ASSERT(static_cast<uint8_t>(BufferOwnership::NotSpecified) == 0);
        ASSERT(static_cast<uint8_t>(BufferOwnership::ExplicitRequired) == 1);
        ASSERT(static_cast<uint8_t>(BufferOwnership::ImplicitAllowed) == 2);
    });
    
    runner.run("realtime_buffer_lifetime_proof_not_specified", []() {
        ASSERT(RealtimeBufferLifetimeProof{RealtimeBufferLifetimeProof::NotSpecified} == RealtimeBufferLifetimeProof::NotSpecified);
    });
    
    runner.run("realtime_buffer_lifetime_proof_across_boundary", []() {
        ASSERT(RealtimeBufferLifetimeProof{RealtimeBufferLifetimeProof::MustBeProvableAcrossBoundary} == RealtimeBufferLifetimeProof::MustBeProvableAcrossBoundary);
    });
    
    runner.run("realtime_buffer_lifetime_proof_within_boundary", []() {
        ASSERT(RealtimeBufferLifetimeProof{RealtimeBufferLifetimeProof::MustBeProvableWithinBoundary} == RealtimeBufferLifetimeProof::MustBeProvableWithinBoundary);
    });
    
    runner.run("realtime_buffer_lifetime_proof_no_requirement", []() {
        ASSERT(RealtimeBufferLifetimeProof{RealtimeBufferLifetimeProof::NoRequirement} == RealtimeBufferLifetimeProof::NoRequirement);
    });
    
    runner.run("realtime_buffer_lifetime_proof_enum_values", []() {
        ASSERT(static_cast<uint8_t>(RealtimeBufferLifetimeProof::NotSpecified) == 0);
        ASSERT(static_cast<uint8_t>(RealtimeBufferLifetimeProof::MustBeProvableAcrossBoundary) == 1);
        ASSERT(static_cast<uint8_t>(RealtimeBufferLifetimeProof::MustBeProvableWithinBoundary) == 2);
        ASSERT(static_cast<uint8_t>(RealtimeBufferLifetimeProof::NoRequirement) == 3);
    });
}
