#pragma once

extern "C" {
#include <libswresample/swresample.h>
}

#include "kivo/core/contract/format/descriptor/audio_format_descriptor.hpp"
#include "kivo/core/contract/format/negotiation/conversion_policy.hpp"
#include "kivo/core/processing/policy/dither_policy.hpp"
#include "kivo/core/processing/policy/resample_quality.hpp"
#include "kivo/core/processing/snapshot/audio_conversion_snapshot.hpp"

namespace kivo::adapters::ffmpeg::detail {

[[nodiscard]] bool configure_ffmpeg_resampler(
    SwrContext& context,
    const core::contract::AudioFormatDescriptor& source,
    const core::contract::AudioFormatDescriptor& target,
    const core::contract::ConversionPolicy& conversion,
    core::processing::ResampleQuality resample_quality,
    core::processing::DitherPolicy dither_policy,
    core::processing::AudioConversionSnapshot& snapshot) noexcept;

} // namespace kivo::adapters::ffmpeg::detail
