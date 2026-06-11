#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class CommandCoalescingPolicy : uint8_t {
    Unknown = 0,
    MergeSeek,
    MergeVolume,
    MergeTrackSwitch,
    None,
    Custom
};

} // namespace kivo::core::contract