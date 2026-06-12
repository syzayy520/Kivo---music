#include "adapters/ffmpeg/conversion/truth/ffmpeg_conversion_delay.hpp"

#include <algorithm>

namespace kivo::adapters::ffmpeg::detail {

void refresh_ffmpeg_conversion_delay(
    SwrContext& context,
    uint32_t sample_rate,
    core::processing::AudioConversionSnapshot& snapshot) noexcept {
    if (sample_rate == 0) {
        snapshot.latency_frames = 0;
        snapshot.pending_tail_frames = 0;
        return;
    }
    const auto delay = swr_get_delay(
        &context,
        static_cast<int64_t>(sample_rate));
    const auto frames = delay > 0
        ? static_cast<core::contract::FrameCount>(delay)
        : 0;
    snapshot.pending_tail_frames = frames;
    snapshot.latency_frames =
        std::max(snapshot.latency_frames, frames);
}

} // namespace kivo::adapters::ffmpeg::detail
