#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class GenerationResetReason : uint8_t {
    NotSpecified = 0,
    Seek,
    Flush,
    StreamRestart,
    DeviceReset,
    FormatChange
};

} // namespace kivo::core::contract