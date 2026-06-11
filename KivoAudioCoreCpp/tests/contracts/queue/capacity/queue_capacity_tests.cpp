#include "../../contract_tests_main.h"
#include "kivo/core/contract/queue/capacity/queue_capacity_policy.hpp"
#include "kivo/core/contract/queue/capacity/queue_overflow_policy.hpp"

namespace {

void queue_capacity_policy_default_is_not_specified() {
    kivo::core::contract::QueueCapacityPolicy policy{};
    ASSERT(policy == kivo::core::contract::QueueCapacityPolicy::NotSpecified);
}

void queue_capacity_policy_fixed() {
    auto policy = kivo::core::contract::QueueCapacityPolicy::Fixed;
    ASSERT(policy != kivo::core::contract::QueueCapacityPolicy::NotSpecified);
}

void queue_capacity_policy_elastic() {
    auto policy = kivo::core::contract::QueueCapacityPolicy::Elastic;
    ASSERT(policy != kivo::core::contract::QueueCapacityPolicy::Fixed);
}

void queue_capacity_policy_bounded() {
    auto policy = kivo::core::contract::QueueCapacityPolicy::Bounded;
    ASSERT(policy != kivo::core::contract::QueueCapacityPolicy::Elastic);
}

void queue_overflow_policy_default_is_not_specified() {
    kivo::core::contract::QueueOverflowPolicy policy{};
    ASSERT(policy == kivo::core::contract::QueueOverflowPolicy::NotSpecified);
}

void queue_overflow_policy_drop_oldest() {
    auto policy = kivo::core::contract::QueueOverflowPolicy::DropOldest;
    ASSERT(policy != kivo::core::contract::QueueOverflowPolicy::NotSpecified);
}

void queue_overflow_policy_drop_newest() {
    auto policy = kivo::core::contract::QueueOverflowPolicy::DropNewest;
    ASSERT(policy != kivo::core::contract::QueueOverflowPolicy::DropOldest);
}

void queue_overflow_policy_block_producer() {
    auto policy = kivo::core::contract::QueueOverflowPolicy::BlockProducer;
    ASSERT(policy != kivo::core::contract::QueueOverflowPolicy::DropNewest);
}

void queue_overflow_policy_reject() {
    auto policy = kivo::core::contract::QueueOverflowPolicy::Reject;
    ASSERT(policy != kivo::core::contract::QueueOverflowPolicy::BlockProducer);
}

void queue_capacity_policy_equality() {
    auto a = kivo::core::contract::QueueCapacityPolicy::Fixed;
    auto b = kivo::core::contract::QueueCapacityPolicy::Fixed;
    ASSERT(a == b);
}

void queue_overflow_policy_equality() {
    auto a = kivo::core::contract::QueueOverflowPolicy::DropOldest;
    auto b = kivo::core::contract::QueueOverflowPolicy::DropOldest;
    ASSERT(a == b);
}

} // namespace

void run_queue_capacity_tests(ContractTestRunner& runner) {
    runner.run("queue_capacity_policy_default_is_not_specified", queue_capacity_policy_default_is_not_specified);
    runner.run("queue_capacity_policy_fixed", queue_capacity_policy_fixed);
    runner.run("queue_capacity_policy_elastic", queue_capacity_policy_elastic);
    runner.run("queue_capacity_policy_bounded", queue_capacity_policy_bounded);
    runner.run("queue_overflow_policy_default_is_not_specified", queue_overflow_policy_default_is_not_specified);
    runner.run("queue_overflow_policy_drop_oldest", queue_overflow_policy_drop_oldest);
    runner.run("queue_overflow_policy_drop_newest", queue_overflow_policy_drop_newest);
    runner.run("queue_overflow_policy_block_producer", queue_overflow_policy_block_producer);
    runner.run("queue_overflow_policy_reject", queue_overflow_policy_reject);
    runner.run("queue_capacity_policy_equality", queue_capacity_policy_equality);
    runner.run("queue_overflow_policy_equality", queue_overflow_policy_equality);
}