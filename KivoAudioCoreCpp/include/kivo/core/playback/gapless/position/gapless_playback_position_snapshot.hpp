#pragma once

#include "kivo/core/contract/sample_position.hpp"

namespace kivo::core::playback {

struct GaplessPlaybackPositionSnapshot {
    contract::SamplePosition decoded_frontier{0};
    contract::SamplePosition queued_frontier{0};
    contract::SamplePosition rendered_frontier{0};
    contract::SamplePosition estimated_audible_frontier{0};

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return estimated_audible_frontier <= rendered_frontier
            && rendered_frontier <= queued_frontier
            && queued_frontier <= decoded_frontier;
    }

    [[nodiscard]] constexpr bool operator==(
        const GaplessPlaybackPositionSnapshot&) const noexcept = default;
};

} // namespace kivo::core::playback
