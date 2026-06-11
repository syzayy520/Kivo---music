#pragma once

#include <cstdint>

#include "trace_event_kind.hpp"

namespace kivo::core::contract::observability {

struct TraceRecord {
    TraceEventKind kind{TraceEventKind::Unknown};
    uint64_t timestamp_ns{0};
    uint64_t duration_ns{0};
    uint64_t trace_id{0};
    uint64_t span_id{0};
    uint64_t parent_span_id{0};

    [[nodiscard]] bool operator==(const TraceRecord&) const noexcept = default;
};

} // namespace kivo::core::contract::observability
