#pragma once

#include <cstdint>

#include "diagnostic_snapshot_kind.hpp"

namespace kivo::core::contract::observability {

struct DiagnosticSnapshot {
    DiagnosticSnapshotKind kind{DiagnosticSnapshotKind::Unknown};
    uint64_t timestamp_ns{0};
    uint64_t snapshot_id{0};
    uint32_t severity_level{0};

    [[nodiscard]] constexpr bool operator==(const DiagnosticSnapshot&) const noexcept = default;
};

} // namespace kivo::core::contract::observability
