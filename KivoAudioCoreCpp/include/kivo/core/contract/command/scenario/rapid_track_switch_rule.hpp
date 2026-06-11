#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class RapidTrackSwitchRule : uint8_t {
    Unknown = 0,
    KeepLatest,
    KeepFirst,
    CancelPrevious,
    QueueSequential,
    RejectRapid
};

} // namespace kivo::core::contract