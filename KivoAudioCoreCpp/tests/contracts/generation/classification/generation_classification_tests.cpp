#include "../../contract_tests_main.h"
#include "kivo/core/contract/generation/classification/stale_generation_classification.hpp"

namespace {

void stale_generation_default_is_not_specified() {
    kivo::core::contract::StaleGenerationClassification cls{};
    ASSERT(cls == kivo::core::contract::StaleGenerationClassification::NotSpecified);
}

void stale_generation_current() {
    auto cls = kivo::core::contract::StaleGenerationClassification::Current;
    ASSERT(cls != kivo::core::contract::StaleGenerationClassification::NotSpecified);
}

void stale_generation_stale() {
    auto cls = kivo::core::contract::StaleGenerationClassification::Stale;
    ASSERT(cls != kivo::core::contract::StaleGenerationClassification::Current);
}

void stale_generation_future() {
    auto cls = kivo::core::contract::StaleGenerationClassification::Future;
    ASSERT(cls != kivo::core::contract::StaleGenerationClassification::Stale);
}

void stale_generation_unknown() {
    auto cls = kivo::core::contract::StaleGenerationClassification::Unknown;
    ASSERT(cls != kivo::core::contract::StaleGenerationClassification::Future);
}

void stale_generation_equality() {
    auto a = kivo::core::contract::StaleGenerationClassification::Current;
    auto b = kivo::core::contract::StaleGenerationClassification::Current;
    ASSERT(a == b);
}

} // namespace

void run_generation_classification_tests(ContractTestRunner& runner) {
    runner.run("stale_generation_default_is_not_specified", stale_generation_default_is_not_specified);
    runner.run("stale_generation_current", stale_generation_current);
    runner.run("stale_generation_stale", stale_generation_stale);
    runner.run("stale_generation_future", stale_generation_future);
    runner.run("stale_generation_unknown", stale_generation_unknown);
    runner.run("stale_generation_equality", stale_generation_equality);
}