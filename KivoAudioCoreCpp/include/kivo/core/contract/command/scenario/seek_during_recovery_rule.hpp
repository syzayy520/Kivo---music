#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class SeekDuringRecoveryRule : uint8_t {
    Unknown = 0,
    QueueAfterRecovery,
    CancelRecovery,
    ReplaceRecovery,
    RejectSeek,
    DeferSeek
};

} // namespace kivo::core::contract