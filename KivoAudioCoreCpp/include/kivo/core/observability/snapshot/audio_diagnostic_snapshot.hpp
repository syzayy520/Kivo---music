#pragma once

#include <array>
#include <cstdint>

#include "kivo/core/observability/category/decode_failure_category.hpp"

namespace kivo::core::observability {

struct AudioDiagnosticSnapshot {
    uint64_t render_underruns{0};
    uint64_t render_protocol_overruns{0};
    uint64_t queue_rejected_full{0};
    uint64_t queue_rejected_oversized{0};
    uint64_t producer_backpressure{0};
    uint64_t device_invalidations{0};
    uint64_t device_reopen_attempts{0};
    uint64_t device_reopen_successes{0};
    uint64_t device_reopen_failures{0};
    uint64_t format_negotiation_attempts{0};
    uint64_t format_negotiation_successes{0};
    uint64_t format_negotiation_failures{0};
    uint64_t format_renegotiation_attempts{0};
    uint64_t format_renegotiation_successes{0};
    uint64_t format_renegotiation_failures{0};
    std::array<uint64_t, kDecodeFailureCategoryCount>
        decode_failures_by_category{};
    uint64_t recovery_attempts{0};
    uint64_t recovery_successes{0};
    uint64_t recovery_failures{0};
    uint64_t recovery_fallback_stops{0};
    uint64_t successful_seeks{0};
    uint64_t drain_attempts{0};
    uint64_t successful_drains{0};
    uint64_t failed_drains{0};
    uint64_t drain_timeouts{0};
    uint64_t stale_blocks_rejected{0};
    uint64_t stale_commands_rejected{0};
    uint64_t source_replacements{0};

    [[nodiscard]] constexpr bool is_consistent() const noexcept {
        return format_negotiation_successes
                <= format_negotiation_attempts
            && format_negotiation_failures
                <= format_negotiation_attempts
                    - format_negotiation_successes
            && format_renegotiation_successes
                <= format_renegotiation_attempts
            && format_renegotiation_failures
                <= format_renegotiation_attempts
                    - format_renegotiation_successes
            && device_reopen_successes <= device_reopen_attempts
            && device_reopen_failures
                <= device_reopen_attempts - device_reopen_successes
            && successful_drains <= drain_attempts
            && failed_drains <= drain_attempts - successful_drains;
    }

    [[nodiscard]] constexpr bool operator==(
        const AudioDiagnosticSnapshot&) const noexcept = default;
};

} // namespace kivo::core::observability
