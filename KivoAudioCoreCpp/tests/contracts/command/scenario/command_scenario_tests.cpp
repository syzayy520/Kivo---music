#include "../../contract_tests_main.h"
#include "kivo/core/contract/command/scenario/rapid_seek_rule.hpp"
#include "kivo/core/contract/command/scenario/rapid_track_switch_rule.hpp"
#include "kivo/core/contract/command/scenario/pause_during_track_switch_rule.hpp"
#include "kivo/core/contract/command/scenario/seek_during_recovery_rule.hpp"
#include "kivo/core/contract/command/scenario/close_during_drain_rule.hpp"
#include "kivo/core/contract/command/scenario/shutdown_during_write_rule.hpp"
#include "kivo/core/contract/command/scenario/library_scan_during_media_rule.hpp"
#include "kivo/core/contract/command/scenario/source_disconnect_during_seek_rule.hpp"
#include "kivo/core/contract/command/scenario/device_lost_during_flush_rule.hpp"

using namespace kivo::core::contract;

namespace {

void rapid_seek_rule_default_is_unknown(ContractTestRunner& runner) {
    runner.run("rapid_seek_rule_default_is_unknown", []() {
        RapidSeekRule r{};
        ASSERT(r == RapidSeekRule::Unknown);
    });
}

void rapid_seek_rule_values(ContractTestRunner& runner) {
    runner.run("rapid_seek_rule_values", []() {
        ASSERT(static_cast<uint8_t>(RapidSeekRule::Unknown) == 0);
        ASSERT(static_cast<uint8_t>(RapidSeekRule::KeepLatest) == 1);
        ASSERT(static_cast<uint8_t>(RapidSeekRule::KeepFirst) == 2);
        ASSERT(static_cast<uint8_t>(RapidSeekRule::MergeAll) == 3);
        ASSERT(static_cast<uint8_t>(RapidSeekRule::RejectIntermediate) == 4);
        ASSERT(static_cast<uint8_t>(RapidSeekRule::QueueAll) == 5);
    });
}

void rapid_track_switch_rule_default_is_unknown(ContractTestRunner& runner) {
    runner.run("rapid_track_switch_rule_default_is_unknown", []() {
        RapidTrackSwitchRule r{};
        ASSERT(r == RapidTrackSwitchRule::Unknown);
    });
}

void rapid_track_switch_rule_values(ContractTestRunner& runner) {
    runner.run("rapid_track_switch_rule_values", []() {
        ASSERT(static_cast<uint8_t>(RapidTrackSwitchRule::Unknown) == 0);
        ASSERT(static_cast<uint8_t>(RapidTrackSwitchRule::KeepLatest) == 1);
        ASSERT(static_cast<uint8_t>(RapidTrackSwitchRule::KeepFirst) == 2);
        ASSERT(static_cast<uint8_t>(RapidTrackSwitchRule::CancelPrevious) == 3);
        ASSERT(static_cast<uint8_t>(RapidTrackSwitchRule::QueueSequential) == 4);
        ASSERT(static_cast<uint8_t>(RapidTrackSwitchRule::RejectRapid) == 5);
    });
}

void pause_during_track_switch_rule_default_is_unknown(ContractTestRunner& runner) {
    runner.run("pause_during_track_switch_rule_default_is_unknown", []() {
        PauseDuringTrackSwitchRule r{};
        ASSERT(r == PauseDuringTrackSwitchRule::Unknown);
    });
}

void pause_during_track_switch_rule_values(ContractTestRunner& runner) {
    runner.run("pause_during_track_switch_rule_values", []() {
        ASSERT(static_cast<uint8_t>(PauseDuringTrackSwitchRule::Unknown) == 0);
        ASSERT(static_cast<uint8_t>(PauseDuringTrackSwitchRule::QueueAfterSwitch) == 1);
        ASSERT(static_cast<uint8_t>(PauseDuringTrackSwitchRule::CancelSwitch) == 2);
        ASSERT(static_cast<uint8_t>(PauseDuringTrackSwitchRule::PauseImmediately) == 3);
        ASSERT(static_cast<uint8_t>(PauseDuringTrackSwitchRule::RejectPause) == 4);
        ASSERT(static_cast<uint8_t>(PauseDuringTrackSwitchRule::DeferPause) == 5);
    });
}

void seek_during_recovery_rule_default_is_unknown(ContractTestRunner& runner) {
    runner.run("seek_during_recovery_rule_default_is_unknown", []() {
        SeekDuringRecoveryRule r{};
        ASSERT(r == SeekDuringRecoveryRule::Unknown);
    });
}

void seek_during_recovery_rule_values(ContractTestRunner& runner) {
    runner.run("seek_during_recovery_rule_values", []() {
        ASSERT(static_cast<uint8_t>(SeekDuringRecoveryRule::Unknown) == 0);
        ASSERT(static_cast<uint8_t>(SeekDuringRecoveryRule::QueueAfterRecovery) == 1);
        ASSERT(static_cast<uint8_t>(SeekDuringRecoveryRule::CancelRecovery) == 2);
        ASSERT(static_cast<uint8_t>(SeekDuringRecoveryRule::ReplaceRecovery) == 3);
        ASSERT(static_cast<uint8_t>(SeekDuringRecoveryRule::RejectSeek) == 4);
        ASSERT(static_cast<uint8_t>(SeekDuringRecoveryRule::DeferSeek) == 5);
    });
}

void close_during_drain_rule_default_is_unknown(ContractTestRunner& runner) {
    runner.run("close_during_drain_rule_default_is_unknown", []() {
        CloseDuringDrainRule r{};
        ASSERT(r == CloseDuringDrainRule::Unknown);
    });
}

void close_during_drain_rule_values(ContractTestRunner& runner) {
    runner.run("close_during_drain_rule_values", []() {
        ASSERT(static_cast<uint8_t>(CloseDuringDrainRule::Unknown) == 0);
        ASSERT(static_cast<uint8_t>(CloseDuringDrainRule::CancelDrainAndClose) == 1);
        ASSERT(static_cast<uint8_t>(CloseDuringDrainRule::WaitForDrain) == 2);
        ASSERT(static_cast<uint8_t>(CloseDuringDrainRule::ForceClose) == 3);
        ASSERT(static_cast<uint8_t>(CloseDuringDrainRule::RejectClose) == 4);
        ASSERT(static_cast<uint8_t>(CloseDuringDrainRule::DeferClose) == 5);
    });
}

void shutdown_during_write_rule_default_is_unknown(ContractTestRunner& runner) {
    runner.run("shutdown_during_write_rule_default_is_unknown", []() {
        ShutdownDuringWriteRule r{};
        ASSERT(r == ShutdownDuringWriteRule::Unknown);
    });
}

void shutdown_during_write_rule_values(ContractTestRunner& runner) {
    runner.run("shutdown_during_write_rule_values", []() {
        ASSERT(static_cast<uint8_t>(ShutdownDuringWriteRule::Unknown) == 0);
        ASSERT(static_cast<uint8_t>(ShutdownDuringWriteRule::CancelWriteAndShutdown) == 1);
        ASSERT(static_cast<uint8_t>(ShutdownDuringWriteRule::DrainThenShutdown) == 2);
        ASSERT(static_cast<uint8_t>(ShutdownDuringWriteRule::ForceShutdown) == 3);
        ASSERT(static_cast<uint8_t>(ShutdownDuringWriteRule::RejectShutdown) == 4);
        ASSERT(static_cast<uint8_t>(ShutdownDuringWriteRule::DeferShutdown) == 5);
    });
}

void library_scan_during_media_rule_default_is_unknown(ContractTestRunner& runner) {
    runner.run("library_scan_during_media_rule_default_is_unknown", []() {
        LibraryScanDuringMediaRule r{};
        ASSERT(r == LibraryScanDuringMediaRule::Unknown);
    });
}

void library_scan_during_media_rule_values(ContractTestRunner& runner) {
    runner.run("library_scan_during_media_rule_values", []() {
        ASSERT(static_cast<uint8_t>(LibraryScanDuringMediaRule::Unknown) == 0);
        ASSERT(static_cast<uint8_t>(LibraryScanDuringMediaRule::PauseMediaAndScan) == 1);
        ASSERT(static_cast<uint8_t>(LibraryScanDuringMediaRule::BackgroundScan) == 2);
        ASSERT(static_cast<uint8_t>(LibraryScanDuringMediaRule::RejectScan) == 3);
        ASSERT(static_cast<uint8_t>(LibraryScanDuringMediaRule::QueueScan) == 4);
        ASSERT(static_cast<uint8_t>(LibraryScanDuringMediaRule::CancelMedia) == 5);
    });
}

void source_disconnect_during_seek_rule_default_is_unknown(ContractTestRunner& runner) {
    runner.run("source_disconnect_during_seek_rule_default_is_unknown", []() {
        SourceDisconnectDuringSeekRule r{};
        ASSERT(r == SourceDisconnectDuringSeekRule::Unknown);
    });
}

void source_disconnect_during_seek_rule_values(ContractTestRunner& runner) {
    runner.run("source_disconnect_during_seek_rule_values", []() {
        ASSERT(static_cast<uint8_t>(SourceDisconnectDuringSeekRule::Unknown) == 0);
        ASSERT(static_cast<uint8_t>(SourceDisconnectDuringSeekRule::CancelSeek) == 1);
        ASSERT(static_cast<uint8_t>(SourceDisconnectDuringSeekRule::RetrySeek) == 2);
        ASSERT(static_cast<uint8_t>(SourceDisconnectDuringSeekRule::FallbackSource) == 3);
        ASSERT(static_cast<uint8_t>(SourceDisconnectDuringSeekRule::RejectSeek) == 4);
        ASSERT(static_cast<uint8_t>(SourceDisconnectDuringSeekRule::QueueRetry) == 5);
    });
}

void device_lost_during_flush_rule_default_is_unknown(ContractTestRunner& runner) {
    runner.run("device_lost_during_flush_rule_default_is_unknown", []() {
        DeviceLostDuringFlushRule r{};
        ASSERT(r == DeviceLostDuringFlushRule::Unknown);
    });
}

void device_lost_during_flush_rule_values(ContractTestRunner& runner) {
    runner.run("device_lost_during_flush_rule_values", []() {
        ASSERT(static_cast<uint8_t>(DeviceLostDuringFlushRule::Unknown) == 0);
        ASSERT(static_cast<uint8_t>(DeviceLostDuringFlushRule::CancelFlush) == 1);
        ASSERT(static_cast<uint8_t>(DeviceLostDuringFlushRule::RetryFlush) == 2);
        ASSERT(static_cast<uint8_t>(DeviceLostDuringFlushRule::ForceFlush) == 3);
        ASSERT(static_cast<uint8_t>(DeviceLostDuringFlushRule::RejectFlush) == 4);
        ASSERT(static_cast<uint8_t>(DeviceLostDuringFlushRule::DeferFlush) == 5);
    });
}

} // namespace

void run_command_scenario_tests(ContractTestRunner& runner) {
    rapid_seek_rule_default_is_unknown(runner);
    rapid_seek_rule_values(runner);
    rapid_track_switch_rule_default_is_unknown(runner);
    rapid_track_switch_rule_values(runner);
    pause_during_track_switch_rule_default_is_unknown(runner);
    pause_during_track_switch_rule_values(runner);
    seek_during_recovery_rule_default_is_unknown(runner);
    seek_during_recovery_rule_values(runner);
    close_during_drain_rule_default_is_unknown(runner);
    close_during_drain_rule_values(runner);
    shutdown_during_write_rule_default_is_unknown(runner);
    shutdown_during_write_rule_values(runner);
    library_scan_during_media_rule_default_is_unknown(runner);
    library_scan_during_media_rule_values(runner);
    source_disconnect_during_seek_rule_default_is_unknown(runner);
    source_disconnect_during_seek_rule_values(runner);
    device_lost_during_flush_rule_default_is_unknown(runner);
    device_lost_during_flush_rule_values(runner);
}
