#pragma once

#include <cstdint>

namespace kivo::core::contract::observability {

enum class DiagnosticSnapshotKind : uint8_t {
    Unknown = 0,
    SystemState,
    Performance,
    ResourceUsage,
    ErrorState,
    Configuration,
    HealthCheck
};

} // namespace kivo::core::contract::observability