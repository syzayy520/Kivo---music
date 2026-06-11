#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class GenerationAdvanceRule : uint8_t {
    NotSpecified = 0,
    AdvanceOnSeek,
    AdvanceOnFlush,
    AdvanceOnDeviceReset,
    AdvanceOnStreamRestart,
    AdvanceOnFormatChange
};

} // namespace kivo::core::contract