#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class DrainingPauseRule : uint8_t {
    Unknown = 0,
    RejectPause,
    QueuePause,
    CompleteDrainFirst,
    ConvertToStop
};

} // namespace kivo::core::contract