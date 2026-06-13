#include "kivo/core/playback/gapless/position/gapless_position_estimator.hpp"

namespace kivo::core::playback {

std::optional<GaplessPlaybackPositionSnapshot>
estimate_gapless_positions(
    contract::SamplePosition decoded_frontier,
    contract::SamplePosition queued_frontier,
    contract::SamplePosition rendered_frontier,
    contract::FrameCount device_buffered_frames) noexcept {
    const auto audible = device_buffered_frames > rendered_frontier
        ? contract::SamplePosition{0}
        : rendered_frontier - device_buffered_frames;
    const GaplessPlaybackPositionSnapshot snapshot{
        decoded_frontier,
        queued_frontier,
        rendered_frontier,
        audible};
    return snapshot.is_valid()
        ? std::optional<GaplessPlaybackPositionSnapshot>{snapshot}
        : std::nullopt;
}

} // namespace kivo::core::playback
