#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class SeekingReentryRule : uint8_t {
    Unknown = 0,
    MergeSeek,
    QueueSeek,
    ReplaceWithLatestSeek,
    RejectNewSeek
};

} // namespace kivo::core::contract