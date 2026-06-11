#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class SourceDisconnectDuringSeekRule : uint8_t {
    Unknown = 0,
    CancelSeek,
    RetrySeek,
    FallbackSource,
    RejectSeek,
    QueueRetry
};

} // namespace kivo::core::contract