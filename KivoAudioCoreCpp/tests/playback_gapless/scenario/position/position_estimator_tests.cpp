#include "../../fixture/gapless_test_assert.hpp"
#include "../../runner/playback_gapless_test_cases.hpp"

#include "kivo/core/playback/gapless/position/gapless_position_estimator.hpp"

namespace playback_gapless_test {

void position_frontiers_are_ordered() {
    using namespace kivo::core::playback;
    const auto snapshot = estimate_gapless_positions(120, 110, 96, 16);
    GAPLESS_ASSERT(snapshot);
    GAPLESS_ASSERT(snapshot->decoded_frontier == 120);
    GAPLESS_ASSERT(snapshot->queued_frontier == 110);
    GAPLESS_ASSERT(snapshot->rendered_frontier == 96);
    GAPLESS_ASSERT(snapshot->estimated_audible_frontier == 80);
    GAPLESS_ASSERT(!estimate_gapless_positions(100, 101, 90, 5));
}

} // namespace playback_gapless_test
