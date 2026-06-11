#pragma once

#include <cstdint>

namespace kivo::core::contract::observability {

enum class TraceEventKind : uint8_t {
    Unknown = 0,
    SpanStart,
    SpanEnd,
    Event,
    Log,
    Metric,
    Error,
    Debug
};

} // namespace kivo::core::contract::observability