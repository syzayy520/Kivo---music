#include "../../contract_tests_main.h"
#include "kivo/core/contract/realtime/boundary/realtime_boundary_contract.hpp"
#include "kivo/core/contract/realtime/boundary/non_realtime_boundary_contract.hpp"

using namespace kivo::core::contract;

void run_realtime_boundary_contract_tests(ContractTestRunner& runner) {
    runner.run("realtime_boundary_contract_default_construction", []() {
        RealtimeBoundaryContract c;
        ASSERT(c.path == RealtimePath::Unknown);
        ASSERT(c.path_context == RealtimePathContext::Unknown);
        ASSERT(c.allocation == AllocationConstraint::NotSpecified);
        ASSERT(c.blocking_io == BlockingIoConstraint::NotSpecified);
        ASSERT(c.locking == LockingConstraint::NotSpecified);
        ASSERT(c.logging == LoggingConstraint::NotSpecified);
        ASSERT(c.callback == CallbackConstraint::NotSpecified);
        ASSERT(c.exception == ExceptionConstraint::NotSpecified);
        ASSERT(c.preallocation == PreallocationRequirement::NotSpecified);
        ASSERT(c.transport == BoundedTransport::NotSpecified);
        ASSERT(c.spsc == SpscSemantics::NotSpecified);
        ASSERT(c.ownership_transfer == OwnershipTransfer::NotSpecified);
        ASSERT(c.ownership_visibility == OwnershipVisibility::NotSpecified);
        ASSERT(c.buffer_ownership == BufferOwnership::NotSpecified);
        ASSERT(c.buffer_lifetime == RealtimeBufferLifetimeProof::NotSpecified);
    });
    
    runner.run("realtime_boundary_contract_is_valid_requires_path", []() {
        RealtimeBoundaryContract c;
        c.path = RealtimePath::Realtime;
        c.path_context = RealtimePathContext::Unknown;
        ASSERT(!c.is_valid());
    });
    
    runner.run("realtime_boundary_contract_is_valid_requires_context", []() {
        RealtimeBoundaryContract c;
        c.path = RealtimePath::Unknown;
        c.path_context = RealtimePathContext::RenderSide;
        ASSERT(!c.is_valid());
    });
    
    runner.run("realtime_boundary_contract_is_valid_requires_both", []() {
        RealtimeBoundaryContract c;
        c.path = RealtimePath::Realtime;
        c.path_context = RealtimePathContext::RenderSide;
        ASSERT(c.is_valid());
    });
    
    runner.run("realtime_boundary_contract_is_valid_nonrealtime", []() {
        RealtimeBoundaryContract c;
        c.path = RealtimePath::NonRealtime;
        c.path_context = RealtimePathContext::ControlSide;
        ASSERT(c.is_valid());
    });
    
    runner.run("realtime_boundary_contract_field_modification", []() {
        RealtimeBoundaryContract c;
        c.path = RealtimePath::Realtime;
        c.path_context = RealtimePathContext::RenderSide;
        c.allocation = AllocationConstraint::Forbidden;
        c.blocking_io = BlockingIoConstraint::Forbidden;
        c.locking = LockingConstraint::Forbidden;
        c.logging = LoggingConstraint::Forbidden;
        c.callback = CallbackConstraint::Forbidden;
        c.exception = ExceptionConstraint::Forbidden;
        c.preallocation = PreallocationRequirement::Required;
        c.transport = BoundedTransport::Required;
        c.spsc = SpscSemantics::Declared;
        c.ownership_transfer = OwnershipTransfer::ExplicitTransferRequired;
        c.ownership_visibility = OwnershipVisibility::MustBeProvable;
        c.buffer_ownership = BufferOwnership::ExplicitRequired;
        c.buffer_lifetime = RealtimeBufferLifetimeProof::MustBeProvableAcrossBoundary;
        
        ASSERT(c.path == RealtimePath::Realtime);
        ASSERT(c.path_context == RealtimePathContext::RenderSide);
        ASSERT(c.allocation == AllocationConstraint::Forbidden);
        ASSERT(c.blocking_io == BlockingIoConstraint::Forbidden);
        ASSERT(c.locking == LockingConstraint::Forbidden);
        ASSERT(c.logging == LoggingConstraint::Forbidden);
        ASSERT(c.callback == CallbackConstraint::Forbidden);
        ASSERT(c.exception == ExceptionConstraint::Forbidden);
        ASSERT(c.preallocation == PreallocationRequirement::Required);
        ASSERT(c.transport == BoundedTransport::Required);
        ASSERT(c.spsc == SpscSemantics::Declared);
        ASSERT(c.ownership_transfer == OwnershipTransfer::ExplicitTransferRequired);
        ASSERT(c.ownership_visibility == OwnershipVisibility::MustBeProvable);
        ASSERT(c.buffer_ownership == BufferOwnership::ExplicitRequired);
        ASSERT(c.buffer_lifetime == RealtimeBufferLifetimeProof::MustBeProvableAcrossBoundary);
    });
    
    runner.run("realtime_boundary_contract_equality", []() {
        RealtimeBoundaryContract a;
        RealtimeBoundaryContract b;
        ASSERT(a == b);
        a.path = RealtimePath::Realtime;
        ASSERT(!(a == b));
        a = RealtimeBoundaryContract{};
        a.ownership_transfer = OwnershipTransfer::ExplicitTransferRequired;
        ASSERT(!(a == b));
    });
    
    runner.run("realtime_boundary_contract_has_all_required_fields", []() {
        RealtimeBoundaryContract c;
        (void)c.path;
        (void)c.path_context;
        (void)c.allocation;
        (void)c.blocking_io;
        (void)c.locking;
        (void)c.logging;
        (void)c.callback;
        (void)c.exception;
        (void)c.preallocation;
        (void)c.transport;
        (void)c.spsc;
        (void)c.ownership_transfer;
        (void)c.ownership_visibility;
        (void)c.buffer_ownership;
        (void)c.buffer_lifetime;
        ASSERT(true);
    });
    
    runner.run("non_realtime_boundary_contract_default_construction", []() {
        NonRealtimeBoundaryContract c;
        ASSERT(!c.may_allocate);
        ASSERT(!c.may_block);
        ASSERT(!c.may_log);
        ASSERT(!c.may_callback);
        ASSERT(!c.may_throw);
    });
    
    runner.run("non_realtime_boundary_contract_all_true", []() {
        NonRealtimeBoundaryContract c;
        c.may_allocate = true;
        c.may_block = true;
        c.may_log = true;
        c.may_callback = true;
        c.may_throw = true;
        ASSERT(c.may_allocate);
        ASSERT(c.may_block);
        ASSERT(c.may_log);
        ASSERT(c.may_callback);
        ASSERT(c.may_throw);
    });
    
    runner.run("non_realtime_boundary_contract_mixed_values", []() {
        NonRealtimeBoundaryContract c;
        c.may_allocate = true;
        c.may_block = false;
        c.may_log = true;
        c.may_callback = false;
        c.may_throw = true;
        ASSERT(c.may_allocate);
        ASSERT(!c.may_block);
        ASSERT(c.may_log);
        ASSERT(!c.may_callback);
        ASSERT(c.may_throw);
    });
    
    runner.run("non_realtime_boundary_contract_equality", []() {
        NonRealtimeBoundaryContract a;
        NonRealtimeBoundaryContract b;
        ASSERT(a == b);
        a.may_allocate = true;
        ASSERT(!(a == b));
    });
    
    runner.run("non_realtime_boundary_contract_has_all_required_fields", []() {
        NonRealtimeBoundaryContract c;
        (void)c.may_allocate;
        (void)c.may_block;
        (void)c.may_log;
        (void)c.may_callback;
        (void)c.may_throw;
        ASSERT(true);
    });
}
