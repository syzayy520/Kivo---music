#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class RecoveringSeekRule : uint8_t {
    Unknown = 0,
    RejectSeek,
    QueueSeek,
    DeferUntilRecovered,
    FailIfRecoveryRequired
};

} // namespace kivo::core::contract