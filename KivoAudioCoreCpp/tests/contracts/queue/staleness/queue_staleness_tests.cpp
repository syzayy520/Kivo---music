#include "../../contract_tests_main.h"
#include "kivo/core/contract/queue/staleness/queue_staleness_threshold.hpp"
#include "kivo/core/contract/queue/staleness/queue_staleness_policy.hpp"

namespace {

void queue_staleness_threshold_default_is_zero() {
    kivo::core::contract::QueueStalenessThreshold threshold;
    ASSERT(threshold.max_age_millis == 0);
}

void queue_staleness_threshold_equality() {
    kivo::core::contract::QueueStalenessThreshold a{100};
    kivo::core::contract::QueueStalenessThreshold b{100};
    ASSERT(a == b);
}

void queue_staleness_threshold_inequality() {
    kivo::core::contract::QueueStalenessThreshold a{100};
    kivo::core::contract::QueueStalenessThreshold b{200};
    ASSERT(!(a == b));
}

void queue_staleness_policy_default_is_not_specified() {
    kivo::core::contract::QueueStalenessPolicy policy{};
    ASSERT(policy == kivo::core::contract::QueueStalenessPolicy::NotSpecified);
}

void queue_staleness_policy_discard_stale() {
    auto policy = kivo::core::contract::QueueStalenessPolicy::DiscardStale;
    ASSERT(policy != kivo::core::contract::QueueStalenessPolicy::NotSpecified);
}

void queue_staleness_policy_keep_stale() {
    auto policy = kivo::core::contract::QueueStalenessPolicy::KeepStale;
    ASSERT(policy != kivo::core::contract::QueueStalenessPolicy::DiscardStale);
}

void queue_staleness_policy_mark_stale() {
    auto policy = kivo::core::contract::QueueStalenessPolicy::MarkStale;
    ASSERT(policy != kivo::core::contract::QueueStalenessPolicy::KeepStale);
}

void queue_staleness_policy_equality() {
    auto a = kivo::core::contract::QueueStalenessPolicy::DiscardStale;
    auto b = kivo::core::contract::QueueStalenessPolicy::DiscardStale;
    ASSERT(a == b);
}

} // namespace

void run_queue_staleness_tests(ContractTestRunner& runner) {
    runner.run("queue_staleness_threshold_default_is_zero", queue_staleness_threshold_default_is_zero);
    runner.run("queue_staleness_threshold_equality", queue_staleness_threshold_equality);
    runner.run("queue_staleness_threshold_inequality", queue_staleness_threshold_inequality);
    runner.run("queue_staleness_policy_default_is_not_specified", queue_staleness_policy_default_is_not_specified);
    runner.run("queue_staleness_policy_discard_stale", queue_staleness_policy_discard_stale);
    runner.run("queue_staleness_policy_keep_stale", queue_staleness_policy_keep_stale);
    runner.run("queue_staleness_policy_mark_stale", queue_staleness_policy_mark_stale);
    runner.run("queue_staleness_policy_equality", queue_staleness_policy_equality);
}