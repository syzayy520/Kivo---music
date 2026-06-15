#pragma once

#include <cstdint>

namespace kivo::core::render {

enum class RenderFailure : uint8_t {
    None = 0,
    Underrun,
    Overrun,
    DeviceLost,
    InvalidFormat,
    UnsupportedFormat,
    StaleStreamGeneration,
    StaleSeekGeneration,
    StaleFlushGeneration,
    StaleDeviceGeneration,
    Timeout,
    InvalidState,
    InvalidBuffer,
    BoundaryFailure
};

} // namespace kivo::core::render
