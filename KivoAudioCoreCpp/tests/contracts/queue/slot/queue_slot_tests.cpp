#include "../../contract_tests_main.h"
#include "kivo/core/contract/queue/slot/queue_slot_state.hpp"
#include "kivo/core/contract/queue/slot/queue_slot_generation.hpp"

namespace {

void queue_slot_state_default_is_not_specified() {
    kivo::core::contract::QueueSlotState state{};
    ASSERT(state == kivo::core::contract::QueueSlotState::NotSpecified);
}

void queue_slot_state_empty() {
    auto state = kivo::core::contract::QueueSlotState::Empty;
    ASSERT(state != kivo::core::contract::QueueSlotState::NotSpecified);
}

void queue_slot_state_filled() {
    auto state = kivo::core::contract::QueueSlotState::Filled;
    ASSERT(state != kivo::core::contract::QueueSlotState::Empty);
}

void queue_slot_state_consumed() {
    auto state = kivo::core::contract::QueueSlotState::Consumed;
    ASSERT(state != kivo::core::contract::QueueSlotState::Filled);
}

void queue_slot_state_reserved() {
    auto state = kivo::core::contract::QueueSlotState::Reserved;
    ASSERT(state != kivo::core::contract::QueueSlotState::Consumed);
}

void queue_slot_generation_default_is_initial() {
    kivo::core::contract::QueueSlotGeneration gen;
    ASSERT(gen.value() == kivo::core::contract::GenerationId::initial().value());
}

void queue_slot_generation_equality() {
    kivo::core::contract::QueueSlotGeneration a;
    kivo::core::contract::QueueSlotGeneration b;
    ASSERT(a == b);
}

void queue_slot_generation_inequality() {
    kivo::core::contract::QueueSlotGeneration a;
    kivo::core::contract::QueueSlotGeneration b{kivo::core::contract::GenerationId::next(a.id)};
    ASSERT(!(a == b));
}

void queue_slot_generation_value_accessor() {
    kivo::core::contract::QueueSlotGeneration gen;
    ASSERT(gen.value() == gen.id.value());
}

void queue_slot_state_equality() {
    auto a = kivo::core::contract::QueueSlotState::Filled;
    auto b = kivo::core::contract::QueueSlotState::Filled;
    ASSERT(a == b);
}

} // namespace

void run_queue_slot_tests(ContractTestRunner& runner) {
    runner.run("queue_slot_state_default_is_not_specified", queue_slot_state_default_is_not_specified);
    runner.run("queue_slot_state_empty", queue_slot_state_empty);
    runner.run("queue_slot_state_filled", queue_slot_state_filled);
    runner.run("queue_slot_state_consumed", queue_slot_state_consumed);
    runner.run("queue_slot_state_reserved", queue_slot_state_reserved);
    runner.run("queue_slot_generation_default_is_initial", queue_slot_generation_default_is_initial);
    runner.run("queue_slot_generation_equality", queue_slot_generation_equality);
    runner.run("queue_slot_generation_inequality", queue_slot_generation_inequality);
    runner.run("queue_slot_generation_value_accessor", queue_slot_generation_value_accessor);
    runner.run("queue_slot_state_equality", queue_slot_state_equality);
}