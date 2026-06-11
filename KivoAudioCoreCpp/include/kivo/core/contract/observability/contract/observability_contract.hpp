#pragma once

#include <cstdint>

#include "../event/observation_severity.hpp"
#include "../trace/trace_buffer_policy.hpp"

namespace kivo::core::contract::observability {

struct ObservabilityContract {
    TraceBufferPolicy trace_buffer_policy{TraceBufferPolicy::Unknown};
    ObservationSeverity min_severity{ObservationSeverity::Unknown};
    bool enable_metrics{false};
    bool enable_tracing{false};
    bool enable_diagnostics{false};
    uint32_t max_metric_samples{0};
    uint32_t max_trace_records{0};
    uint32_t max_diagnostic_snapshots{0};

    [[nodiscard]] bool operator==(const ObservabilityContract&) const noexcept = default;
};

} // namespace kivo::core::contract::observability
