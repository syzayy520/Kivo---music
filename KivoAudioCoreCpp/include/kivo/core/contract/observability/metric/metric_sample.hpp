#pragma once

#include <cstdint>

#include "metric_kind.hpp"
#include "metric_unit.hpp"

namespace kivo::core::contract::observability {

struct MetricSample {
    MetricKind kind{MetricKind::Unknown};
    MetricUnit unit{MetricUnit::Unknown};
    uint64_t timestamp_ns{0};
    double value{0.0};
    uint32_t sample_count{0};

    bool operator==(const MetricSample&) const noexcept = default;
};

} // namespace kivo::core::contract::observability