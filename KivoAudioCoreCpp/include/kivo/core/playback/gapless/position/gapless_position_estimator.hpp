#pragma once

#include <optional>

#include "kivo/core/playback/gapless/position/gapless_playback_position_snapshot.hpp"

namespace kivo::core::playback {

[[nodiscard]] std::optional<GaplessPlaybackPositionSnapshot>
estimate_gapless_positions(
    contract::SamplePosition decoded_frontier,
    contract::SamplePosition queued_frontier,
    contract::SamplePosition rendered_frontier,
    contract::FrameCount device_buffered_frames) noexcept;

} // namespace kivo::core::playback
