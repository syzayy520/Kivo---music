#include "kivo/core/playback/gapless/transition/gapless_transition_evaluator.hpp"

namespace kivo::core::playback {

GaplessTransitionDecision evaluate_gapless_transition(
    const GaplessTrackPlan& current,
    const GaplessTrackPlan& next) noexcept {
    if (!current.is_valid()) {
        return {
            false,
            GaplessTransitionRejectionReason::InvalidCurrentPlan,
            contract::kInvalidSamplePosition};
    }
    if (!next.is_valid()) {
        return {
            false,
            GaplessTransitionRejectionReason::InvalidNextPlan,
            current.timeline_end()};
    }
    const auto boundary = current.timeline_end();
    if (current.render_format != next.render_format) {
        return {
            false,
            GaplessTransitionRejectionReason::FormatMismatch,
            boundary};
    }
    if (next.timeline_origin > boundary) {
        return {
            false,
            GaplessTransitionRejectionReason::TimelineGap,
            boundary};
    }
    if (next.timeline_origin < boundary) {
        return {
            false,
            GaplessTransitionRejectionReason::TimelineOverlap,
            boundary};
    }
    return {
        true,
        GaplessTransitionRejectionReason::None,
        boundary};
}

} // namespace kivo::core::playback
