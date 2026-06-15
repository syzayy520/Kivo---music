#include "../../contract_tests_main.h"
#include "kivo/core/contract/command/kind/command_kind.hpp"
#include "kivo/core/contract/command/kind/command_origin.hpp"
#include "kivo/core/contract/command/kind/command_priority.hpp"

using namespace kivo::core::contract;

namespace {

void command_kind_default_is_unknown(ContractTestRunner& runner) {
    runner.run("command_kind_default_is_unknown", []() {
        CommandKind k{};
        ASSERT(k == CommandKind::Unknown);
    });
}

void command_kind_values(ContractTestRunner& runner) {
    runner.run("command_kind_values", []() {
        ASSERT(static_cast<uint8_t>(CommandKind::Unknown) == 0);
        ASSERT(static_cast<uint8_t>(CommandKind::OpenSource) == 1);
        ASSERT(static_cast<uint8_t>(CommandKind::CloseSource) == 2);
        ASSERT(static_cast<uint8_t>(CommandKind::Seek) == 3);
        ASSERT(static_cast<uint8_t>(CommandKind::Pause) == 4);
        ASSERT(static_cast<uint8_t>(CommandKind::Resume) == 5);
        ASSERT(static_cast<uint8_t>(CommandKind::Stop) == 6);
        ASSERT(static_cast<uint8_t>(CommandKind::SkipForward) == 7);
        ASSERT(static_cast<uint8_t>(CommandKind::SkipBackward) == 8);
        ASSERT(static_cast<uint8_t>(CommandKind::Flush) == 9);
        ASSERT(static_cast<uint8_t>(CommandKind::SetVolume) == 10);
        ASSERT(static_cast<uint8_t>(CommandKind::SetOutput) == 11);
        ASSERT(static_cast<uint8_t>(CommandKind::Shutdown) == 12);
    });
}

void command_kind_equality(ContractTestRunner& runner) {
    runner.run("command_kind_equality", []() {
        ASSERT(CommandKind::Seek == CommandKind::Seek);
        ASSERT(CommandKind::Unknown != CommandKind::Seek);
    });
}

void command_origin_default_is_unknown(ContractTestRunner& runner) {
    runner.run("command_origin_default_is_unknown", []() {
        CommandOrigin o{};
        ASSERT(o == CommandOrigin::Unknown);
    });
}

void command_origin_values(ContractTestRunner& runner) {
    runner.run("command_origin_values", []() {
        ASSERT(static_cast<uint8_t>(CommandOrigin::Unknown) == 0);
        ASSERT(static_cast<uint8_t>(CommandOrigin::User) == 1);
        ASSERT(static_cast<uint8_t>(CommandOrigin::System) == 2);
        ASSERT(static_cast<uint8_t>(CommandOrigin::Recovery) == 3);
        ASSERT(static_cast<uint8_t>(CommandOrigin::Scheduler) == 4);
        ASSERT(static_cast<uint8_t>(CommandOrigin::External) == 5);
    });
}

void command_origin_equality(ContractTestRunner& runner) {
    runner.run("command_origin_equality", []() {
        ASSERT(CommandOrigin::User == CommandOrigin::User);
        ASSERT(CommandOrigin::Unknown != CommandOrigin::User);
    });
}

void command_priority_default_is_unknown(ContractTestRunner& runner) {
    runner.run("command_priority_default_is_unknown", []() {
        CommandPriority p{};
        ASSERT(p == CommandPriority::Unknown);
    });
}

void command_priority_values(ContractTestRunner& runner) {
    runner.run("command_priority_values", []() {
        ASSERT(static_cast<uint8_t>(CommandPriority::Unknown) == 0);
        ASSERT(static_cast<uint8_t>(CommandPriority::Critical) == 1);
        ASSERT(static_cast<uint8_t>(CommandPriority::High) == 2);
        ASSERT(static_cast<uint8_t>(CommandPriority::Normal) == 3);
        ASSERT(static_cast<uint8_t>(CommandPriority::Low) == 4);
    });
}

void command_priority_equality(ContractTestRunner& runner) {
    runner.run("command_priority_equality", []() {
        ASSERT(CommandPriority::Normal == CommandPriority::Normal);
        ASSERT(CommandPriority::Unknown != CommandPriority::Normal);
    });
}

} // namespace

void run_command_kind_tests(ContractTestRunner& runner) {
    command_kind_default_is_unknown(runner);
    command_kind_values(runner);
    command_kind_equality(runner);
    command_origin_default_is_unknown(runner);
    command_origin_values(runner);
    command_origin_equality(runner);
    command_priority_default_is_unknown(runner);
    command_priority_values(runner);
    command_priority_equality(runner);
}
