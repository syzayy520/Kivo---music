#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class CommandSupersessionPolicy : uint8_t {
    Unknown = 0,
    CancelPending,
    CancelRunning,
    QueueBehind,
    RejectNew,
    ReplacePending
};

} // namespace kivo::core::contract