#pragma once

#include <cstdint>

#include "kivo/core/contract/format/descriptor/audio_format_descriptor.hpp"

namespace hires_pcm_test {

[[nodiscard]] constexpr kivo::core::contract::AudioFormatDescriptor make_pcm_format(
    kivo::core::contract::SampleFormat sample_format,
    kivo::core::contract::ChannelLayout channel_layout,
    uint32_t sample_rate) noexcept {
    return {
        sample_format,
        channel_layout,
        kivo::core::contract::FrameLayout::Interleaved,
        {},
        sample_rate,
        kivo::core::contract::sample_format_valid_bits(sample_format),
        0
    };
}

} // namespace hires_pcm_test
