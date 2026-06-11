#include "../../contract_tests_main.h"
#include "kivo/core/contract/command/ordering/command_ordering_policy.hpp"
#include "kivo/core/contract/command/ordering/command_supersession_policy.hpp"
#include "kivo/core/contract/command/ordering/command_coalescing_policy.hpp"
#include "kivo/core/contract/command/ordering/command_ordering_contract.hpp"

using namespace kivo::core::contract;

namespace {

void ordering_policy_default_is_unknown(ContractTestRunner& runner) {
    runner.run("ordering_policy_default_is_unknown", []() {
        CommandOrderingPolicy p{};
        ASSERT(p == CommandOrderingPolicy::Unknown);
    });
}

void ordering_policy_values(ContractTestRunner& runner) {
    runner.run("ordering_policy_values", []() {
        ASSERT(static_cast<uint8_t>(CommandOrderingPolicy::Unknown) == 0);
        ASSERT(static_cast<uint8_t>(CommandOrderingPolicy::Fifo) == 1);
        ASSERT(static_cast<uint8_t>(CommandOrderingPolicy::Lifo) == 2);
        ASSERT(static_cast<uint8_t>(CommandOrderingPolicy::PriorityOrdered) == 3);
        ASSERT(static_cast<uint8_t>(CommandOrderingPolicy::DeadlineOrdered) == 4);
        ASSERT(static_cast<uint8_t>(CommandOrderingPolicy::None) == 5);
    });
}

void ordering_policy_equality(ContractTestRunner& runner) {
    runner.run("ordering_policy_equality", []() {
        ASSERT(CommandOrderingPolicy::Fifo == CommandOrderingPolicy::Fifo);
        ASSERT(CommandOrderingPolicy::Unknown != CommandOrderingPolicy::Fifo);
    });
}

void supersession_policy_default_is_unknown(ContractTestRunner& runner) {
    runner.run("supersession_policy_default_is_unknown", []() {
        CommandSupersessionPolicy p{};
        ASSERT(p == CommandSupersessionPolicy::Unknown);
    });
}

void supersession_policy_values(ContractTestRunner& runner) {
    runner.run("supersession_policy_values", []() {
        ASSERT(static_cast<uint8_t>(CommandSupersessionPolicy::Unknown) == 0);
        ASSERT(static_cast<uint8_t>(CommandSupersessionPolicy::CancelPending) == 1);
        ASSERT(static_cast<uint8_t>(CommandSupersessionPolicy::CancelRunning) == 2);
        ASSERT(static_cast<uint8_t>(CommandSupersessionPolicy::QueueBehind) == 3);
        ASSERT(static_cast<uint8_t>(CommandSupersessionPolicy::RejectNew) == 4);
        ASSERT(static_cast<uint8_t>(CommandSupersessionPolicy::ReplacePending) == 5);
    });
}

void supersession_policy_equality(ContractTestRunner& runner) {
    runner.run("supersession_policy_equality", []() {
        ASSERT(CommandSupersessionPolicy::CancelPending == CommandSupersessionPolicy::CancelPending);
        ASSERT(CommandSupersessionPolicy::Unknown != CommandSupersessionPolicy::CancelPending);
    });
}

void coalescing_policy_default_is_unknown(ContractTestRunner& runner) {
    runner.run("coalescing_policy_default_is_unknown", []() {
        CommandCoalescingPolicy p{};
        ASSERT(p == CommandCoalescingPolicy::Unknown);
    });
}

void coalescing_policy_values(ContractTestRunner& runner) {
    runner.run("coalescing_policy_values", []() {
        ASSERT(static_cast<uint8_t>(CommandCoalescingPolicy::Unknown) == 0);
        ASSERT(static_cast<uint8_t>(CommandCoalescingPolicy::MergeSeek) == 1);
        ASSERT(static_cast<uint8_t>(CommandCoalescingPolicy::MergeVolume) == 2);
        ASSERT(static_cast<uint8_t>(CommandCoalescingPolicy::MergeTrackSwitch) == 3);
        ASSERT(static_cast<uint8_t>(CommandCoalescingPolicy::None) == 4);
        ASSERT(static_cast<uint8_t>(CommandCoalescingPolicy::Custom) == 5);
    });
}

void coalescing_policy_equality(ContractTestRunner& runner) {
    runner.run("coalescing_policy_equality", []() {
        ASSERT(CommandCoalescingPolicy::MergeSeek == CommandCoalescingPolicy::MergeSeek);
        ASSERT(CommandCoalescingPolicy::Unknown != CommandCoalescingPolicy::MergeSeek);
    });
}

void ordering_contract_default_construction(ContractTestRunner& runner) {
    runner.run("ordering_contract_default_construction", []() {
        CommandOrderingContract c{};
        ASSERT(c.ordering == CommandOrderingPolicy::Unknown);
        ASSERT(c.supersession == CommandSupersessionPolicy::Unknown);
        ASSERT(c.coalescing == CommandCoalescingPolicy::Unknown);
    });
}

void ordering_contract_field_modification(ContractTestRunner& runner) {
    runner.run("ordering_contract_field_modification", []() {
        CommandOrderingContract c{};
        c.ordering = CommandOrderingPolicy::Fifo;
        c.supersession = CommandSupersessionPolicy::CancelPending;
        c.coalescing = CommandCoalescingPolicy::MergeSeek;
        ASSERT(c.ordering == CommandOrderingPolicy::Fifo);
        ASSERT(c.supersession == CommandSupersessionPolicy::CancelPending);
        ASSERT(c.coalescing == CommandCoalescingPolicy::MergeSeek);
    });
}

void ordering_contract_equality(ContractTestRunner& runner) {
    runner.run("ordering_contract_equality", []() {
        CommandOrderingContract a{};
        CommandOrderingContract b{};
        ASSERT(a == b);

        CommandOrderingContract c{};
        c.ordering = CommandOrderingPolicy::Fifo;
        ASSERT(!(a == c));
    });
}

void ordering_contract_has_all_fields(ContractTestRunner& runner) {
    runner.run("ordering_contract_has_all_fields", []() {
        CommandOrderingContract c{};
        (void)c.ordering;
        (void)c.supersession;
        (void)c.coalescing;
        ASSERT(true);
    });
}

} // namespace

void run_command_ordering_tests(ContractTestRunner& runner) {
    ordering_policy_default_is_unknown(runner);
    ordering_policy_values(runner);
    ordering_policy_equality(runner);
    supersession_policy_default_is_unknown(runner);
    supersession_policy_values(runner);
    supersession_policy_equality(runner);
    coalescing_policy_default_is_unknown(runner);
    coalescing_policy_values(runner);
    coalescing_policy_equality(runner);
    ordering_contract_default_construction(runner);
    ordering_contract_field_modification(runner);
    ordering_contract_equality(runner);
    ordering_contract_has_all_fields(runner);
}
