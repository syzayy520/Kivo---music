#pragma once

#include <cstdint>

namespace kivo::core::contract::observability {

enum class ObservationEventKind : uint8_t {
    Unknown = 0,
    MetricSample,
    StateChange,
    ErrorOccurred,
    Warning,
    Info,
    Debug,
    Trace
};

} // namespace kivo::core::contract::observability