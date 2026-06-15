#pragma once

#include "kivo/core/playback/gapless/plan/gapless_track_plan.hpp"
#include "kivo/core/playback/gapless/transition/gapless_transition_decision.hpp"

namespace kivo::core::playback {

[[nodiscard]] GaplessTransitionDecision evaluate_gapless_transition(
    const GaplessTrackPlan& current,
    const GaplessTrackPlan& next) noexcept;

} // namespace kivo::core::playback
