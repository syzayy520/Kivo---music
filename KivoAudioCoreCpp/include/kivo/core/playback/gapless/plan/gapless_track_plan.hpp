#pragma once

#include <cstdint>
#include <limits>

#include "kivo/core/contract/cue/continuity/gapless_metadata.hpp"
#include "kivo/core/contract/format/roles/render_format.hpp"
#include "kivo/core/contract/sample_position.hpp"

namespace kivo::core::playback {

struct GaplessTrackPlan {
    uint32_t track_index{0};
    contract::FrameCount audible_frames{0};
    contract::SamplePosition timeline_origin{0};
    contract::RenderFormat render_format{};
    contract::GaplessMetadata metadata{};
    bool final_track{false};

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return audible_frames != 0
            && timeline_origin != contract::kInvalidSamplePosition
            && render_format.is_valid()
            && audible_frames
                <= std::numeric_limits<contract::SamplePosition>::max()
                    - timeline_origin;
    }

    [[nodiscard]] constexpr contract::SamplePosition
    timeline_end() const noexcept {
        return is_valid()
            ? timeline_origin + audible_frames
            : contract::kInvalidSamplePosition;
    }

    [[nodiscard]] constexpr bool operator==(
        const GaplessTrackPlan&) const noexcept = default;
};

} // namespace kivo::core::playback
