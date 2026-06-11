#include "../../contract_tests_main.h"
#include "kivo/core/contract/command/identity/command_id.hpp"
#include "kivo/core/contract/command/identity/command_generation_ref.hpp"

using namespace kivo::core::contract;

namespace {

void command_id_default_construction(ContractTestRunner& runner) {
    runner.run("command_id_default_construction", []() {
        CommandId id{};
        ASSERT(id.value == 0);
    });
}

void command_id_field_modification(ContractTestRunner& runner) {
    runner.run("command_id_field_modification", []() {
        CommandId id{};
        id.value = 42;
        ASSERT(id.value == 42);
    });
}

void command_id_equality(ContractTestRunner& runner) {
    runner.run("command_id_equality", []() {
        CommandId a{};
        CommandId b{};
        ASSERT(a == b);

        CommandId c{};
        c.value = 7;
        ASSERT(!(a == c));
    });
}

void command_generation_ref_default_construction(ContractTestRunner& runner) {
    runner.run("command_generation_ref_default_construction", []() {
        CommandGenerationRef ref{};
        ASSERT(ref.generation == 0);
        ASSERT(ref.origin == CommandOrigin::Unknown);
    });
}

void command_generation_ref_field_modification(ContractTestRunner& runner) {
    runner.run("command_generation_ref_field_modification", []() {
        CommandGenerationRef ref{};
        ref.generation = 5;
        ref.origin = CommandOrigin::User;
        ASSERT(ref.generation == 5);
        ASSERT(ref.origin == CommandOrigin::User);
    });
}

void command_generation_ref_equality(ContractTestRunner& runner) {
    runner.run("command_generation_ref_equality", []() {
        CommandGenerationRef a{};
        CommandGenerationRef b{};
        ASSERT(a == b);

        CommandGenerationRef c{};
        c.generation = 3;
        ASSERT(!(a == c));
    });
}

void command_generation_ref_has_all_fields(ContractTestRunner& runner) {
    runner.run("command_generation_ref_has_all_fields", []() {
        CommandGenerationRef ref{};
        (void)ref.generation;
        (void)ref.origin;
        ASSERT(true);
    });
}

} // namespace

void run_command_identity_tests(ContractTestRunner& runner) {
    command_id_default_construction(runner);
    command_id_field_modification(runner);
    command_id_equality(runner);
    command_generation_ref_default_construction(runner);
    command_generation_ref_field_modification(runner);
    command_generation_ref_equality(runner);
    command_generation_ref_has_all_fields(runner);
}
