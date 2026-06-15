#pragma once

#include <cstdint>

extern "C" {
#include <libswresample/swresample.h>
}

#include "kivo/core/processing/snapshot/audio_conversion_snapshot.hpp"

namespace kivo::adapters::ffmpeg::detail {

void refresh_ffmpeg_conversion_delay(
    SwrContext& context,
    uint32_t sample_rate,
    core::processing::AudioConversionSnapshot& snapshot) noexcept;

} // namespace kivo::adapters::ffmpeg::detail
