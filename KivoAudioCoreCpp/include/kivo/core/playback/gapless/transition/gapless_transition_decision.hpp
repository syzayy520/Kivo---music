#pragma once

#include <cstdint>

#include "kivo/core/contract/sample_position.hpp"

namespace kivo::core::playback {

enum class GaplessTransitionRejectionReason : uint8_t {
    None = 0,
    InvalidCurrentPlan,
    InvalidNextPlan,
    FormatMismatch,
    TimelineGap,
    TimelineOverlap
};

struct GaplessTransitionDecision {
    bool compatible{false};
    GaplessTransitionRejectionReason rejection{
        GaplessTransitionRejectionReason::InvalidCurrentPlan};
    contract::SamplePosition boundary{
        contract::kInvalidSamplePosition};

    [[nodiscard]] constexpr bool operator==(
        const GaplessTransitionDecision&) const noexcept = default;
};

} // namespace kivo::core::playback
