#include "../../contract_tests_main.h"
#include "kivo/core/contract/generation/comparison/generation_match_rule.hpp"

namespace {

void generation_match_rule_default_is_not_specified() {
    kivo::core::contract::GenerationMatchRule rule{};
    ASSERT(rule == kivo::core::contract::GenerationMatchRule::NotSpecified);
}

void generation_match_rule_must_match() {
    auto rule = kivo::core::contract::GenerationMatchRule::MustMatch;
    ASSERT(rule != kivo::core::contract::GenerationMatchRule::NotSpecified);
}

void generation_match_rule_may_differ() {
    auto rule = kivo::core::contract::GenerationMatchRule::MayDiffer;
    ASSERT(rule != kivo::core::contract::GenerationMatchRule::MustMatch);
}

void generation_match_rule_must_be_newer() {
    auto rule = kivo::core::contract::GenerationMatchRule::MustBeNewer;
    ASSERT(rule != kivo::core::contract::GenerationMatchRule::MayDiffer);
}

void generation_match_rule_equality() {
    auto a = kivo::core::contract::GenerationMatchRule::MustMatch;
    auto b = kivo::core::contract::GenerationMatchRule::MustMatch;
    ASSERT(a == b);
}

void generation_match_rule_inequality() {
    auto a = kivo::core::contract::GenerationMatchRule::MustMatch;
    auto b = kivo::core::contract::GenerationMatchRule::MustBeNewer;
    ASSERT(a != b);
}

} // namespace

void run_generation_comparison_tests(ContractTestRunner& runner) {
    runner.run("generation_match_rule_default_is_not_specified", generation_match_rule_default_is_not_specified);
    runner.run("generation_match_rule_must_match", generation_match_rule_must_match);
    runner.run("generation_match_rule_may_differ", generation_match_rule_may_differ);
    runner.run("generation_match_rule_must_be_newer", generation_match_rule_must_be_newer);
    runner.run("generation_match_rule_equality", generation_match_rule_equality);
    runner.run("generation_match_rule_inequality", generation_match_rule_inequality);
}