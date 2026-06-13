#include "core/processing/gain/audio_gain_application.hpp"

#include <cmath>
#include <cstdint>
#include <limits>

#include "core/processing/gain/audio_gain_sample_math.hpp"

namespace kivo::core::processing::detail {

namespace {

template <typename Sample>
GainApplicationResult apply_integer(
    std::span<std::byte> bytes,
    double gain,
    bool dither,
    uint64_t& state,
    double minimum,
    double maximum,
    int shift) noexcept {
    uint64_t clipped = 0;
    for (size_t offset = 0; offset < bytes.size(); offset += sizeof(Sample)) {
        const auto packed = load_sample<Sample>(bytes.data() + offset);
        const auto value = shift == 0
            ? static_cast<double>(packed)
            : static_cast<double>(packed / (int64_t{1} << shift));
        const auto noise = dither ? triangular_dither(state) : 0.0;
        const auto quantized = quantize_integer<int64_t>(
            value * gain + noise,
            minimum,
            maximum,
            clipped);
        const auto stored = shift == 0
            ? quantized
            : quantized * (int64_t{1} << shift);
        store_sample(
            bytes.data() + offset,
            static_cast<Sample>(stored));
    }
    return {true, clipped};
}

template <typename Sample>
GainApplicationResult apply_float(
    std::span<std::byte> bytes,
    double gain) noexcept {
    uint64_t clipped = 0;
    for (size_t offset = 0; offset < bytes.size(); offset += sizeof(Sample)) {
        auto value = static_cast<double>(
            load_sample<Sample>(bytes.data() + offset));
        value *= gain;
        if (!std::isfinite(value)) {
            ++clipped;
            value = 0.0;
        } else if (value < -1.0) {
            ++clipped;
            value = -1.0;
        } else if (value > 1.0) {
            ++clipped;
            value = 1.0;
        }
        store_sample(
            bytes.data() + offset,
            static_cast<Sample>(value));
    }
    return {true, clipped};
}

} // namespace

GainApplicationResult apply_gain(
    std::span<std::byte> bytes,
    contract::RenderFormat format,
    contract::FrameCount frame_count,
    double gain,
    bool dither,
    uint64_t& dither_state) noexcept {
    const auto channels = format.format.channel_count();
    const auto bytes_per_sample = format.format.container_bits() / 8u;
    if (!format.is_valid()
        || format.format.frame_layout != contract::FrameLayout::Interleaved
        || channels == 0
        || bytes_per_sample == 0
        || frame_count
            > std::numeric_limits<contract::FrameCount>::max() / channels) {
        return {};
    }
    const auto sample_count = frame_count * channels;
    if (sample_count
            > std::numeric_limits<size_t>::max() / bytes_per_sample
        || bytes.size() != static_cast<size_t>(sample_count) * bytes_per_sample) {
        return {};
    }

    using contract::SampleFormat;
    switch (format.format.sample_format) {
    case SampleFormat::Int16:
        return apply_integer<int16_t>(
            bytes, gain, dither, dither_state, -32768.0, 32767.0, 0);
    case SampleFormat::Int24:
        return apply_integer<int32_t>(
            bytes, gain, dither, dither_state, -8388608.0, 8388607.0, 8);
    case SampleFormat::Int32:
        return apply_integer<int32_t>(
            bytes,
            gain,
            dither,
            dither_state,
            static_cast<double>(std::numeric_limits<int32_t>::min()),
            static_cast<double>(std::numeric_limits<int32_t>::max()),
            0);
    case SampleFormat::Float32:
        return apply_float<float>(bytes, gain);
    case SampleFormat::Float64:
        return apply_float<double>(bytes, gain);
    default:
        return {};
    }
}

} // namespace kivo::core::processing::detail
