#include "../../contract_tests_main.h"
#include "kivo/core/contract/error/recovery/recovery_action.hpp"

using namespace kivo::core::contract;

namespace {

void recovery_action_default_is_none(ContractTestRunner& runner) {
    runner.run("recovery_action_default_is_none", []() {
        RecoveryAction a{};
        ASSERT(a == RecoveryAction::None);
    });
}

void recovery_action_retry(ContractTestRunner& runner) {
    runner.run("recovery_action_retry", []() {
        ASSERT(static_cast<uint8_t>(RecoveryAction::Retry) == 1);
    });
}

void recovery_action_drain(ContractTestRunner& runner) {
    runner.run("recovery_action_drain", []() {
        ASSERT(static_cast<uint8_t>(RecoveryAction::Drain) == 2);
    });
}

void recovery_action_flush(ContractTestRunner& runner) {
    runner.run("recovery_action_flush", []() {
        ASSERT(static_cast<uint8_t>(RecoveryAction::Flush) == 3);
    });
}

void recovery_action_reopen_device(ContractTestRunner& runner) {
    runner.run("recovery_action_reopen_device", []() {
        ASSERT(static_cast<uint8_t>(RecoveryAction::ReopenDevice) == 4);
    });
}

void recovery_action_reconfigure_format(ContractTestRunner& runner) {
    runner.run("recovery_action_reconfigure_format", []() {
        ASSERT(static_cast<uint8_t>(RecoveryAction::ReconfigureFormat) == 5);
    });
}

void recovery_action_skip_frame(ContractTestRunner& runner) {
    runner.run("recovery_action_skip_frame", []() {
        ASSERT(static_cast<uint8_t>(RecoveryAction::SkipFrame) == 6);
    });
}

void recovery_action_skip_track(ContractTestRunner& runner) {
    runner.run("recovery_action_skip_track", []() {
        ASSERT(static_cast<uint8_t>(RecoveryAction::SkipTrack) == 7);
    });
}

void recovery_action_stop_media(ContractTestRunner& runner) {
    runner.run("recovery_action_stop_media", []() {
        ASSERT(static_cast<uint8_t>(RecoveryAction::StopMedia) == 8);
    });
}

void recovery_action_report_to_user(ContractTestRunner& runner) {
    runner.run("recovery_action_report_to_user", []() {
        ASSERT(static_cast<uint8_t>(RecoveryAction::ReportToUser) == 9);
    });
}

void recovery_action_rebuild_source(ContractTestRunner& runner) {
    runner.run("recovery_action_rebuild_source", []() {
        ASSERT(static_cast<uint8_t>(RecoveryAction::RebuildSource) == 10);
    });
}

void recovery_action_rebuild_codec(ContractTestRunner& runner) {
    runner.run("recovery_action_rebuild_codec", []() {
        ASSERT(static_cast<uint8_t>(RecoveryAction::RebuildCodec) == 11);
    });
}

void recovery_action_rebuild_output(ContractTestRunner& runner) {
    runner.run("recovery_action_rebuild_output", []() {
        ASSERT(static_cast<uint8_t>(RecoveryAction::RebuildOutput) == 12);
    });
}

void recovery_action_equality(ContractTestRunner& runner) {
    runner.run("recovery_action_equality", []() {
        ASSERT(RecoveryAction::Retry == RecoveryAction::Retry);
        ASSERT(RecoveryAction::None != RecoveryAction::Retry);
    });
}

} // namespace

void run_recovery_action_tests(ContractTestRunner& runner) {
    recovery_action_default_is_none(runner);
    recovery_action_retry(runner);
    recovery_action_drain(runner);
    recovery_action_flush(runner);
    recovery_action_reopen_device(runner);
    recovery_action_reconfigure_format(runner);
    recovery_action_skip_frame(runner);
    recovery_action_skip_track(runner);
    recovery_action_stop_media(runner);
    recovery_action_report_to_user(runner);
    recovery_action_rebuild_source(runner);
    recovery_action_rebuild_codec(runner);
    recovery_action_rebuild_output(runner);
    recovery_action_equality(runner);
}