#include "../../contract_tests_main.h"
#include "kivo/core/contract/realtime/transport/bounded_transport_rule.hpp"
#include "kivo/core/contract/realtime/transport/spsc_semantics_rule.hpp"

using namespace kivo::core::contract;

void run_realtime_transport_contract_tests(ContractTestRunner& runner) {
    runner.run("bounded_transport_not_specified", []() {
        ASSERT(BoundedTransport{BoundedTransport::NotSpecified} == BoundedTransport::NotSpecified);
    });
    
    runner.run("bounded_transport_required", []() {
        ASSERT(BoundedTransport{BoundedTransport::Required} == BoundedTransport::Required);
    });
    
    runner.run("bounded_transport_recommended", []() {
        ASSERT(BoundedTransport{BoundedTransport::Recommended} == BoundedTransport::Recommended);
    });
    
    runner.run("bounded_transport_not_required", []() {
        ASSERT(BoundedTransport{BoundedTransport::NotRequired} == BoundedTransport::NotRequired);
    });
    
    runner.run("bounded_transport_enum_values", []() {
        ASSERT(static_cast<uint8_t>(BoundedTransport::NotSpecified) == 0);
        ASSERT(static_cast<uint8_t>(BoundedTransport::Required) == 1);
        ASSERT(static_cast<uint8_t>(BoundedTransport::Recommended) == 2);
        ASSERT(static_cast<uint8_t>(BoundedTransport::NotRequired) == 3);
    });
    
    runner.run("spsc_semantics_not_specified", []() {
        ASSERT(SpscSemantics{SpscSemantics::NotSpecified} == SpscSemantics::NotSpecified);
    });
    
    runner.run("spsc_semantics_declared", []() {
        ASSERT(SpscSemantics{SpscSemantics::Declared} == SpscSemantics::Declared);
    });
    
    runner.run("spsc_semantics_not_declared", []() {
        ASSERT(SpscSemantics{SpscSemantics::NotDeclared} == SpscSemantics::NotDeclared);
    });
    
    runner.run("spsc_semantics_enum_values", []() {
        ASSERT(static_cast<uint8_t>(SpscSemantics::NotSpecified) == 0);
        ASSERT(static_cast<uint8_t>(SpscSemantics::Declared) == 1);
        ASSERT(static_cast<uint8_t>(SpscSemantics::NotDeclared) == 2);
    });
}