#pragma once

#include <array>
#include <cstddef>
#include <cstring>

#include "kivo/core/contract/format/roles/render_format.hpp"

namespace audio_processing_test {

[[nodiscard]] constexpr kivo::core::contract::RenderFormat format(
    kivo::core::contract::SampleFormat sample_format) noexcept {
    using namespace kivo::core::contract;
    return {{
        sample_format,
        ChannelLayout::Stereo,
        FrameLayout::Interleaved,
        {},
        48000,
        sample_format_valid_bits(sample_format),
        sample_format_container_bits(sample_format)
    }};
}

template <typename Sample, size_t Size>
[[nodiscard]] std::array<std::byte, sizeof(Sample) * Size> bytes(
    const std::array<Sample, Size>& samples) noexcept {
    std::array<std::byte, sizeof(Sample) * Size> result{};
    std::memcpy(result.data(), samples.data(), result.size());
    return result;
}

template <typename Sample, size_t ByteSize>
[[nodiscard]] std::array<Sample, ByteSize / sizeof(Sample)> samples(
    const std::array<std::byte, ByteSize>& value) noexcept {
    std::array<Sample, ByteSize / sizeof(Sample)> result{};
    std::memcpy(result.data(), value.data(), value.size());
    return result;
}

} // namespace audio_processing_test
