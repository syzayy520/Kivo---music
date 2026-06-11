#pragma once

#include <cstdint>

namespace kivo::core::contract::observability {

enum class MetricKind : uint8_t {
    Unknown = 0,
    Counter,
    Gauge,
    Histogram,
    Summary,
    Timer
};

} // namespace kivo::core::contract::observability