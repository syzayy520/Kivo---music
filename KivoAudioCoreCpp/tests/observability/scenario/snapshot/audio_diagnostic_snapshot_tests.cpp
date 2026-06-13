#include "../../fixture/observability_test_assert.hpp"
#include "../../runner/observability_test_cases.hpp"

#include <cstdint>
#include <limits>

#include "kivo/core/observability/snapshot/audio_diagnostic_snapshot.hpp"

namespace observability_test {

void diagnostic_snapshot_rejects_impossible_totals() {
    using kivo::core::observability::AudioDiagnosticSnapshot;

    AudioDiagnosticSnapshot snapshot{};
    OBSERVABILITY_ASSERT(snapshot.is_consistent());

    snapshot.format_negotiation_attempts = 2;
    snapshot.format_negotiation_successes = 1;
    snapshot.format_negotiation_failures = 1;
    snapshot.device_reopen_attempts = 3;
    snapshot.device_reopen_successes = 2;
    snapshot.device_reopen_failures = 1;
    snapshot.drain_attempts = 4;
    snapshot.successful_drains = 3;
    snapshot.failed_drains = 1;
    OBSERVABILITY_ASSERT(snapshot.is_consistent());

    snapshot.format_negotiation_failures = 2;
    OBSERVABILITY_ASSERT(!snapshot.is_consistent());

    snapshot = {};
    snapshot.format_negotiation_attempts =
        std::numeric_limits<uint64_t>::max();
    snapshot.format_negotiation_successes =
        std::numeric_limits<uint64_t>::max();
    OBSERVABILITY_ASSERT(snapshot.is_consistent());
}

} // namespace observability_test
