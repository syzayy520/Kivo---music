#include "../../contract_tests_main.h"
#include "kivo/core/contract/generation/advance/generation_reset_reason.hpp"
#include "kivo/core/contract/generation/advance/generation_advance_rule.hpp"

namespace {

void generation_reset_reason_default_is_not_specified() {
    kivo::core::contract::GenerationResetReason reason{};
    ASSERT(reason == kivo::core::contract::GenerationResetReason::NotSpecified);
}

void generation_reset_reason_seek() {
    auto reason = kivo::core::contract::GenerationResetReason::Seek;
    ASSERT(reason != kivo::core::contract::GenerationResetReason::NotSpecified);
}

void generation_reset_reason_flush() {
    auto reason = kivo::core::contract::GenerationResetReason::Flush;
    ASSERT(reason != kivo::core::contract::GenerationResetReason::Seek);
}

void generation_reset_reason_all_values() {
    auto r1 = kivo::core::contract::GenerationResetReason::StreamRestart;
    auto r2 = kivo::core::contract::GenerationResetReason::DeviceReset;
    auto r3 = kivo::core::contract::GenerationResetReason::FormatChange;
    ASSERT(r1 != r2);
    ASSERT(r2 != r3);
    ASSERT(r1 != r3);
}

void generation_advance_rule_default_is_not_specified() {
    kivo::core::contract::GenerationAdvanceRule rule{};
    ASSERT(rule == kivo::core::contract::GenerationAdvanceRule::NotSpecified);
}

void generation_advance_rule_advance_on_seek() {
    auto rule = kivo::core::contract::GenerationAdvanceRule::AdvanceOnSeek;
    ASSERT(rule != kivo::core::contract::GenerationAdvanceRule::NotSpecified);
}

void generation_advance_rule_advance_on_flush() {
    auto rule = kivo::core::contract::GenerationAdvanceRule::AdvanceOnFlush;
    ASSERT(rule != kivo::core::contract::GenerationAdvanceRule::AdvanceOnSeek);
}

void generation_advance_rule_all_values() {
    auto r1 = kivo::core::contract::GenerationAdvanceRule::AdvanceOnDeviceReset;
    auto r2 = kivo::core::contract::GenerationAdvanceRule::AdvanceOnStreamRestart;
    auto r3 = kivo::core::contract::GenerationAdvanceRule::AdvanceOnFormatChange;
    ASSERT(r1 != r2);
    ASSERT(r2 != r3);
    ASSERT(r1 != r3);
}

void generation_reset_reason_equality() {
    auto a = kivo::core::contract::GenerationResetReason::Seek;
    auto b = kivo::core::contract::GenerationResetReason::Seek;
    ASSERT(a == b);
}

void generation_advance_rule_equality() {
    auto a = kivo::core::contract::GenerationAdvanceRule::AdvanceOnSeek;
    auto b = kivo::core::contract::GenerationAdvanceRule::AdvanceOnSeek;
    ASSERT(a == b);
}

} // namespace

void run_generation_advance_tests(ContractTestRunner& runner) {
    runner.run("generation_reset_reason_default_is_not_specified", generation_reset_reason_default_is_not_specified);
    runner.run("generation_reset_reason_seek", generation_reset_reason_seek);
    runner.run("generation_reset_reason_flush", generation_reset_reason_flush);
    runner.run("generation_reset_reason_all_values", generation_reset_reason_all_values);
    runner.run("generation_advance_rule_default_is_not_specified", generation_advance_rule_default_is_not_specified);
    runner.run("generation_advance_rule_advance_on_seek", generation_advance_rule_advance_on_seek);
    runner.run("generation_advance_rule_advance_on_flush", generation_advance_rule_advance_on_flush);
    runner.run("generation_advance_rule_all_values", generation_advance_rule_all_values);
    runner.run("generation_reset_reason_equality", generation_reset_reason_equality);
    runner.run("generation_advance_rule_equality", generation_advance_rule_equality);
}