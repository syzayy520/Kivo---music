#include "../../contract_tests_main.h"
#include "kivo/core/contract/realtime/prohibition/realtime_allocation_rule.hpp"
#include "kivo/core/contract/realtime/prohibition/realtime_blocking_io_rule.hpp"
#include "kivo/core/contract/realtime/prohibition/realtime_locking_rule.hpp"
#include "kivo/core/contract/realtime/prohibition/realtime_logging_rule.hpp"
#include "kivo/core/contract/realtime/prohibition/realtime_callback_rule.hpp"
#include "kivo/core/contract/realtime/prohibition/realtime_exception_rule.hpp"

using namespace kivo::core::contract;

void run_realtime_prohibition_contract_tests(ContractTestRunner& runner) {
    runner.run("allocation_constraint_not_specified", []() {
        AllocationConstraint c{AllocationConstraint::NotSpecified};
        ASSERT(c == AllocationConstraint::NotSpecified);
        ASSERT(!(c == AllocationConstraint::Forbidden));
        ASSERT(!(c == AllocationConstraint::Allowed));
        ASSERT(!(c == AllocationConstraint::Conditional));
    });
    
    runner.run("allocation_constraint_forbidden", []() {
        ASSERT(AllocationConstraint{AllocationConstraint::Forbidden} == AllocationConstraint::Forbidden);
    });
    
    runner.run("allocation_constraint_allowed", []() {
        ASSERT(AllocationConstraint{AllocationConstraint::Allowed} == AllocationConstraint::Allowed);
    });
    
    runner.run("allocation_constraint_conditional", []() {
        ASSERT(AllocationConstraint{AllocationConstraint::Conditional} == AllocationConstraint::Conditional);
    });
    
    runner.run("allocation_constraint_enum_values", []() {
        ASSERT(static_cast<uint8_t>(AllocationConstraint::NotSpecified) == 0);
        ASSERT(static_cast<uint8_t>(AllocationConstraint::Forbidden) == 1);
        ASSERT(static_cast<uint8_t>(AllocationConstraint::Allowed) == 2);
        ASSERT(static_cast<uint8_t>(AllocationConstraint::Conditional) == 3);
    });
    
    runner.run("blocking_io_constraint_not_specified", []() {
        ASSERT(BlockingIoConstraint{BlockingIoConstraint::NotSpecified} == BlockingIoConstraint::NotSpecified);
    });
    
    runner.run("blocking_io_constraint_forbidden", []() {
        ASSERT(BlockingIoConstraint{BlockingIoConstraint::Forbidden} == BlockingIoConstraint::Forbidden);
    });
    
    runner.run("blocking_io_constraint_allowed", []() {
        ASSERT(BlockingIoConstraint{BlockingIoConstraint::Allowed} == BlockingIoConstraint::Allowed);
    });
    
    runner.run("blocking_io_constraint_conditional", []() {
        ASSERT(BlockingIoConstraint{BlockingIoConstraint::Conditional} == BlockingIoConstraint::Conditional);
    });
    
    runner.run("blocking_io_constraint_enum_values", []() {
        ASSERT(static_cast<uint8_t>(BlockingIoConstraint::NotSpecified) == 0);
        ASSERT(static_cast<uint8_t>(BlockingIoConstraint::Forbidden) == 1);
        ASSERT(static_cast<uint8_t>(BlockingIoConstraint::Allowed) == 2);
        ASSERT(static_cast<uint8_t>(BlockingIoConstraint::Conditional) == 3);
    });
    
    runner.run("locking_constraint_not_specified", []() {
        ASSERT(LockingConstraint{LockingConstraint::NotSpecified} == LockingConstraint::NotSpecified);
    });
    
    runner.run("locking_constraint_forbidden", []() {
        ASSERT(LockingConstraint{LockingConstraint::Forbidden} == LockingConstraint::Forbidden);
    });
    
    runner.run("locking_constraint_allowed", []() {
        ASSERT(LockingConstraint{LockingConstraint::Allowed} == LockingConstraint::Allowed);
    });
    
    runner.run("locking_constraint_conditional", []() {
        ASSERT(LockingConstraint{LockingConstraint::Conditional} == LockingConstraint::Conditional);
    });
    
    runner.run("locking_constraint_enum_values", []() {
        ASSERT(static_cast<uint8_t>(LockingConstraint::NotSpecified) == 0);
        ASSERT(static_cast<uint8_t>(LockingConstraint::Forbidden) == 1);
        ASSERT(static_cast<uint8_t>(LockingConstraint::Allowed) == 2);
        ASSERT(static_cast<uint8_t>(LockingConstraint::Conditional) == 3);
    });
    
    runner.run("logging_constraint_not_specified", []() {
        ASSERT(LoggingConstraint{LoggingConstraint::NotSpecified} == LoggingConstraint::NotSpecified);
    });
    
    runner.run("logging_constraint_forbidden", []() {
        ASSERT(LoggingConstraint{LoggingConstraint::Forbidden} == LoggingConstraint::Forbidden);
    });
    
    runner.run("logging_constraint_allowed", []() {
        ASSERT(LoggingConstraint{LoggingConstraint::Allowed} == LoggingConstraint::Allowed);
    });
    
    runner.run("logging_constraint_conditional", []() {
        ASSERT(LoggingConstraint{LoggingConstraint::Conditional} == LoggingConstraint::Conditional);
    });
    
    runner.run("logging_constraint_enum_values", []() {
        ASSERT(static_cast<uint8_t>(LoggingConstraint::NotSpecified) == 0);
        ASSERT(static_cast<uint8_t>(LoggingConstraint::Forbidden) == 1);
        ASSERT(static_cast<uint8_t>(LoggingConstraint::Allowed) == 2);
        ASSERT(static_cast<uint8_t>(LoggingConstraint::Conditional) == 3);
    });
    
    runner.run("callback_constraint_not_specified", []() {
        ASSERT(CallbackConstraint{CallbackConstraint::NotSpecified} == CallbackConstraint::NotSpecified);
    });
    
    runner.run("callback_constraint_forbidden", []() {
        ASSERT(CallbackConstraint{CallbackConstraint::Forbidden} == CallbackConstraint::Forbidden);
    });
    
    runner.run("callback_constraint_allowed", []() {
        ASSERT(CallbackConstraint{CallbackConstraint::Allowed} == CallbackConstraint::Allowed);
    });
    
    runner.run("callback_constraint_conditional", []() {
        ASSERT(CallbackConstraint{CallbackConstraint::Conditional} == CallbackConstraint::Conditional);
    });
    
    runner.run("callback_constraint_enum_values", []() {
        ASSERT(static_cast<uint8_t>(CallbackConstraint::NotSpecified) == 0);
        ASSERT(static_cast<uint8_t>(CallbackConstraint::Forbidden) == 1);
        ASSERT(static_cast<uint8_t>(CallbackConstraint::Allowed) == 2);
        ASSERT(static_cast<uint8_t>(CallbackConstraint::Conditional) == 3);
    });
    
    runner.run("exception_constraint_not_specified", []() {
        ASSERT(ExceptionConstraint{ExceptionConstraint::NotSpecified} == ExceptionConstraint::NotSpecified);
    });
    
    runner.run("exception_constraint_forbidden", []() {
        ASSERT(ExceptionConstraint{ExceptionConstraint::Forbidden} == ExceptionConstraint::Forbidden);
    });
    
    runner.run("exception_constraint_allowed", []() {
        ASSERT(ExceptionConstraint{ExceptionConstraint::Allowed} == ExceptionConstraint::Allowed);
    });
    
    runner.run("exception_constraint_conditional", []() {
        ASSERT(ExceptionConstraint{ExceptionConstraint::Conditional} == ExceptionConstraint::Conditional);
    });
    
    runner.run("exception_constraint_enum_values", []() {
        ASSERT(static_cast<uint8_t>(ExceptionConstraint::NotSpecified) == 0);
        ASSERT(static_cast<uint8_t>(ExceptionConstraint::Forbidden) == 1);
        ASSERT(static_cast<uint8_t>(ExceptionConstraint::Allowed) == 2);
        ASSERT(static_cast<uint8_t>(ExceptionConstraint::Conditional) == 3);
    });
}