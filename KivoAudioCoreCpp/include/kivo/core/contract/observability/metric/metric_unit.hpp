#pragma once

#include <cstdint>

namespace kivo::core::contract::observability {

enum class MetricUnit : uint8_t {
    Unknown = 0,
    None,
    Bytes,
    Milliseconds,
    Seconds,
    Percent,
    Count,
    Ratio
};

} // namespace kivo::core::contract::observability