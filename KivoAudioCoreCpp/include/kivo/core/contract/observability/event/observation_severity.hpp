#pragma once

#include <cstdint>

namespace kivo::core::contract::observability {

enum class ObservationSeverity : uint8_t {
    Unknown = 0,
    Critical,
    Error,
    Warning,
    Info,
    Debug,
    Trace
};

} // namespace kivo::core::contract::observability