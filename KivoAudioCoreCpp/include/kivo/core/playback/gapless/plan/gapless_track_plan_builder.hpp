#pragma once

#include <optional>

#include "kivo/core/decode/media/media_probe.hpp"
#include "kivo/core/playback/gapless/plan/gapless_track_plan.hpp"

namespace kivo::core::playback {

[[nodiscard]] constexpr std::optional<GaplessTrackPlan>
build_gapless_track_plan(
    const decode::MediaProbe& probe,
    uint32_t track_index,
    contract::SamplePosition timeline_origin,
    bool final_track,
    contract::Preroll preroll = {},
    contract::Prefetch prefetch = {}) noexcept {
    if (!probe.is_valid()
        || !probe.duration_known
        || probe.duration_frames == 0) {
        return std::nullopt;
    }
    const GaplessTrackPlan plan{
        track_index,
        probe.duration_frames,
        timeline_origin,
        probe.output_format,
        {
            probe.codec_delay,
            probe.trailing_padding,
            preroll,
            prefetch
        },
        final_track
    };
    return plan.is_valid()
        ? std::optional<GaplessTrackPlan>{plan}
        : std::nullopt;
}

} // namespace kivo::core::playback
