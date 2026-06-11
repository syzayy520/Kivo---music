#include "../../contract_tests_main.h"
#include "kivo/core/contract/command/contract/command_semantics_contract.hpp"

using namespace kivo::core::contract;

namespace {

void command_semantics_contract_default_construction(ContractTestRunner& runner) {
    runner.run("command_semantics_contract_default_construction", []() {
        CommandSemanticsContract c{};
        ASSERT(c.ordering.ordering == CommandOrderingPolicy::Unknown);
        ASSERT(c.ordering.supersession == CommandSupersessionPolicy::Unknown);
        ASSERT(c.ordering.coalescing == CommandCoalescingPolicy::Unknown);
        ASSERT(c.rapid_seek == RapidSeekRule::Unknown);
        ASSERT(c.rapid_track_switch == RapidTrackSwitchRule::Unknown);
        ASSERT(c.pause_during_track_switch == PauseDuringTrackSwitchRule::Unknown);
        ASSERT(c.seek_during_recovery == SeekDuringRecoveryRule::Unknown);
        ASSERT(c.close_during_drain == CloseDuringDrainRule::Unknown);
        ASSERT(c.shutdown_during_write == ShutdownDuringWriteRule::Unknown);
        ASSERT(c.library_scan_during_media == LibraryScanDuringMediaRule::Unknown);
        ASSERT(c.source_disconnect_during_seek == SourceDisconnectDuringSeekRule::Unknown);
        ASSERT(c.device_lost_during_flush == DeviceLostDuringFlushRule::Unknown);
    });
}

void command_semantics_contract_field_modification(ContractTestRunner& runner) {
    runner.run("command_semantics_contract_field_modification", []() {
        CommandSemanticsContract c{};
        c.ordering.ordering = CommandOrderingPolicy::Fifo;
        c.ordering.supersession = CommandSupersessionPolicy::CancelPending;
        c.ordering.coalescing = CommandCoalescingPolicy::MergeSeek;
        c.rapid_seek = RapidSeekRule::KeepLatest;
        c.rapid_track_switch = RapidTrackSwitchRule::CancelPrevious;
        c.pause_during_track_switch = PauseDuringTrackSwitchRule::PauseImmediately;
        c.seek_during_recovery = SeekDuringRecoveryRule::QueueAfterRecovery;
        c.close_during_drain = CloseDuringDrainRule::CancelDrainAndClose;
        c.shutdown_during_write = ShutdownDuringWriteRule::DrainThenShutdown;
        c.library_scan_during_media = LibraryScanDuringMediaRule::BackgroundScan;
        c.source_disconnect_during_seek = SourceDisconnectDuringSeekRule::RetrySeek;
        c.device_lost_during_flush = DeviceLostDuringFlushRule::RetryFlush;

        ASSERT(c.ordering.ordering == CommandOrderingPolicy::Fifo);
        ASSERT(c.ordering.supersession == CommandSupersessionPolicy::CancelPending);
        ASSERT(c.ordering.coalescing == CommandCoalescingPolicy::MergeSeek);
        ASSERT(c.rapid_seek == RapidSeekRule::KeepLatest);
        ASSERT(c.rapid_track_switch == RapidTrackSwitchRule::CancelPrevious);
        ASSERT(c.pause_during_track_switch == PauseDuringTrackSwitchRule::PauseImmediately);
        ASSERT(c.seek_during_recovery == SeekDuringRecoveryRule::QueueAfterRecovery);
        ASSERT(c.close_during_drain == CloseDuringDrainRule::CancelDrainAndClose);
        ASSERT(c.shutdown_during_write == ShutdownDuringWriteRule::DrainThenShutdown);
        ASSERT(c.library_scan_during_media == LibraryScanDuringMediaRule::BackgroundScan);
        ASSERT(c.source_disconnect_during_seek == SourceDisconnectDuringSeekRule::RetrySeek);
        ASSERT(c.device_lost_during_flush == DeviceLostDuringFlushRule::RetryFlush);
    });
}

void command_semantics_contract_equality(ContractTestRunner& runner) {
    runner.run("command_semantics_contract_equality", []() {
        CommandSemanticsContract a{};
        CommandSemanticsContract b{};
        ASSERT(a == b);

        CommandSemanticsContract c{};
        c.rapid_seek = RapidSeekRule::KeepLatest;
        ASSERT(!(a == c));
    });
}

void command_semantics_contract_has_all_required_fields(ContractTestRunner& runner) {
    runner.run("command_semantics_contract_has_all_required_fields", []() {
        CommandSemanticsContract c{};
        // Ordering contract (3 fields)
        (void)c.ordering.ordering;
        (void)c.ordering.supersession;
        (void)c.ordering.coalescing;
        // Scenario rules (9 fields)
        (void)c.rapid_seek;
        (void)c.rapid_track_switch;
        (void)c.pause_during_track_switch;
        (void)c.seek_during_recovery;
        (void)c.close_during_drain;
        (void)c.shutdown_during_write;
        (void)c.library_scan_during_media;
        (void)c.source_disconnect_during_seek;
        (void)c.device_lost_during_flush;
        ASSERT(true);
    });
}

} // namespace

void run_command_semantics_contract_tests(ContractTestRunner& runner) {
    command_semantics_contract_default_construction(runner);
    command_semantics_contract_field_modification(runner);
    command_semantics_contract_equality(runner);
    command_semantics_contract_has_all_required_fields(runner);
}
