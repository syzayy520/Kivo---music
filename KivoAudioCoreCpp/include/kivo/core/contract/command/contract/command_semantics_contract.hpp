#pragma once

#include "../ordering/command_ordering_contract.hpp"
#include "../scenario/rapid_seek_rule.hpp"
#include "../scenario/rapid_track_switch_rule.hpp"
#include "../scenario/pause_during_track_switch_rule.hpp"
#include "../scenario/seek_during_recovery_rule.hpp"
#include "../scenario/close_during_drain_rule.hpp"
#include "../scenario/shutdown_during_write_rule.hpp"
#include "../scenario/library_scan_during_media_rule.hpp"
#include "../scenario/source_disconnect_during_seek_rule.hpp"
#include "../scenario/device_lost_during_flush_rule.hpp"

namespace kivo::core::contract {

struct CommandSemanticsContract {
    CommandOrderingContract ordering{};
    RapidSeekRule rapid_seek{RapidSeekRule::Unknown};
    RapidTrackSwitchRule rapid_track_switch{RapidTrackSwitchRule::Unknown};
    PauseDuringTrackSwitchRule pause_during_track_switch{PauseDuringTrackSwitchRule::Unknown};
    SeekDuringRecoveryRule seek_during_recovery{SeekDuringRecoveryRule::Unknown};
    CloseDuringDrainRule close_during_drain{CloseDuringDrainRule::Unknown};
    ShutdownDuringWriteRule shutdown_during_write{ShutdownDuringWriteRule::Unknown};
    LibraryScanDuringMediaRule library_scan_during_media{LibraryScanDuringMediaRule::Unknown};
    SourceDisconnectDuringSeekRule source_disconnect_during_seek{SourceDisconnectDuringSeekRule::Unknown};
    DeviceLostDuringFlushRule device_lost_during_flush{DeviceLostDuringFlushRule::Unknown};

    [[nodiscard]] constexpr bool operator==(const CommandSemanticsContract&) const noexcept = default;
};

} // namespace kivo::core::contract
