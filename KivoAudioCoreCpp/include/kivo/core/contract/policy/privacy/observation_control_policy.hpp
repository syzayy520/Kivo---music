#pragma once
#include <cstdint>

namespace kivo::core::contract::policy {

struct ObservationControlPolicy {
    uint8_t allow_metric_export{0};
    uint8_t allow_trace_export{0};
    uint8_t allow_diagnostic_export{0};

    [[nodiscard]] bool operator==(const ObservationControlPolicy&) const noexcept = default;
};

} // namespace kivo::core::contract::policy
