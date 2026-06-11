#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class ShutdownDuringWriteRule : uint8_t {
    Unknown = 0,
    CancelWriteAndShutdown,
    DrainThenShutdown,
    ForceShutdown,
    RejectShutdown,
    DeferShutdown
};

} // namespace kivo::core::contract