#include "../../contract_tests_main.h"
#include "kivo/core/contract/command/decision/command_decision.hpp"
#include "kivo/core/contract/command/decision/command_rejection_reason.hpp"

using namespace kivo::core::contract;

namespace {

void command_decision_default_is_unknown(ContractTestRunner& runner) {
    runner.run("command_decision_default_is_unknown", []() {
        CommandDecision d{};
        ASSERT(d == CommandDecision::Unknown);
    });
}

void command_decision_values(ContractTestRunner& runner) {
    runner.run("command_decision_values", []() {
        ASSERT(static_cast<uint8_t>(CommandDecision::Unknown) == 0);
        ASSERT(static_cast<uint8_t>(CommandDecision::Accept) == 1);
        ASSERT(static_cast<uint8_t>(CommandDecision::Reject) == 2);
        ASSERT(static_cast<uint8_t>(CommandDecision::Replace) == 3);
        ASSERT(static_cast<uint8_t>(CommandDecision::Supersede) == 4);
        ASSERT(static_cast<uint8_t>(CommandDecision::Coalesce) == 5);
        ASSERT(static_cast<uint8_t>(CommandDecision::Queue) == 6);
        ASSERT(static_cast<uint8_t>(CommandDecision::Defer) == 7);
    });
}

void command_decision_equality(ContractTestRunner& runner) {
    runner.run("command_decision_equality", []() {
        ASSERT(CommandDecision::Accept == CommandDecision::Accept);
        ASSERT(CommandDecision::Unknown != CommandDecision::Accept);
    });
}

void command_rejection_reason_default_is_unknown(ContractTestRunner& runner) {
    runner.run("command_rejection_reason_default_is_unknown", []() {
        CommandRejectionReason r{};
        ASSERT(r == CommandRejectionReason::Unknown);
    });
}

void command_rejection_reason_values(ContractTestRunner& runner) {
    runner.run("command_rejection_reason_values", []() {
        ASSERT(static_cast<uint8_t>(CommandRejectionReason::Unknown) == 0);
        ASSERT(static_cast<uint8_t>(CommandRejectionReason::StaleGeneration) == 1);
        ASSERT(static_cast<uint8_t>(CommandRejectionReason::DuplicateKind) == 2);
        ASSERT(static_cast<uint8_t>(CommandRejectionReason::PriorityConflict) == 3);
        ASSERT(static_cast<uint8_t>(CommandRejectionReason::PolicyViolation) == 4);
        ASSERT(static_cast<uint8_t>(CommandRejectionReason::ResourceExhausted) == 5);
        ASSERT(static_cast<uint8_t>(CommandRejectionReason::InvalidTransition) == 6);
        ASSERT(static_cast<uint8_t>(CommandRejectionReason::DeviceLost) == 7);
        ASSERT(static_cast<uint8_t>(CommandRejectionReason::SourceUnavailable) == 8);
        ASSERT(static_cast<uint8_t>(CommandRejectionReason::ShutdownInProgress) == 9);
    });
}

void command_rejection_reason_equality(ContractTestRunner& runner) {
    runner.run("command_rejection_reason_equality", []() {
        ASSERT(CommandRejectionReason::StaleGeneration == CommandRejectionReason::StaleGeneration);
        ASSERT(CommandRejectionReason::Unknown != CommandRejectionReason::StaleGeneration);
    });
}

} // namespace

void run_command_decision_tests(ContractTestRunner& runner) {
    command_decision_default_is_unknown(runner);
    command_decision_values(runner);
    command_decision_equality(runner);
    command_rejection_reason_default_is_unknown(runner);
    command_rejection_reason_values(runner);
    command_rejection_reason_equality(runner);
}
