#include "../../contract_tests_main.h"
#include "kivo/core/contract/buffer/reference/buffer_reference_type.hpp"

namespace {

void buffer_reference_type_default_is_not_specified() {
    kivo::core::contract::BufferReferenceType type{};
    ASSERT(type == kivo::core::contract::BufferReferenceType::NotSpecified);
}

void buffer_reference_type_strong() {
    auto type = kivo::core::contract::BufferReferenceType::Strong;
    ASSERT(type != kivo::core::contract::BufferReferenceType::NotSpecified);
}

void buffer_reference_type_weak() {
    auto type = kivo::core::contract::BufferReferenceType::Weak;
    ASSERT(type != kivo::core::contract::BufferReferenceType::Strong);
}

void buffer_reference_type_observer() {
    auto type = kivo::core::contract::BufferReferenceType::Observer;
    ASSERT(type != kivo::core::contract::BufferReferenceType::Weak);
}

void buffer_reference_type_equality() {
    auto a = kivo::core::contract::BufferReferenceType::Strong;
    auto b = kivo::core::contract::BufferReferenceType::Strong;
    ASSERT(a == b);
}

void buffer_reference_type_inequality() {
    auto a = kivo::core::contract::BufferReferenceType::Strong;
    auto b = kivo::core::contract::BufferReferenceType::Weak;
    ASSERT(a != b);
}

} // namespace

void run_buffer_reference_tests(ContractTestRunner& runner) {
    runner.run("buffer_reference_type_default_is_not_specified", buffer_reference_type_default_is_not_specified);
    runner.run("buffer_reference_type_strong", buffer_reference_type_strong);
    runner.run("buffer_reference_type_weak", buffer_reference_type_weak);
    runner.run("buffer_reference_type_observer", buffer_reference_type_observer);
    runner.run("buffer_reference_type_equality", buffer_reference_type_equality);
    runner.run("buffer_reference_type_inequality", buffer_reference_type_inequality);
}