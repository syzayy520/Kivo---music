#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class RecoveryAction : uint8_t {
    None = 0,
    Retry,
    Drain,
    Flush,
    ReopenDevice,
    ReconfigureFormat,
    SkipFrame,
    SkipTrack,
    StopMedia,
    ReportToUser,
    RebuildSource,
    RebuildCodec,
    RebuildOutput
};

} // namespace kivo::core::contract