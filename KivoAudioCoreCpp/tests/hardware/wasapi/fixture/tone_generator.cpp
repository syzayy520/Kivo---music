#include "tone_generator.hpp"

#include <cmath>
#include <cstdint>
#include <cstring>
#include <numbers>

namespace kivo::hardware_test::wasapi {

namespace {

template <typename Sample>
void store_sample(std::byte* destination, Sample sample) noexcept {
    std::memcpy(destination, &sample, sizeof(Sample));
}

} // namespace

core::contract::RenderFormat preferred_tone_format() noexcept {
    using namespace core::contract;
    return {{
        SampleFormat::Float32,
        ChannelLayout::Stereo,
        FrameLayout::Interleaved,
        {},
        48000,
        32,
        32
    }};
}

bool fill_tone(
    std::span<std::byte> bytes,
    const core::contract::RenderFormat& format,
    core::contract::FrameCount first_frame,
    core::contract::FrameCount frame_count,
    double frequency_hz,
    double amplitude) noexcept {
    using core::contract::SampleFormat;

    const auto channels = format.format.channel_count();
    const auto bytes_per_sample = format.format.container_bits() / 8u;
    const auto bytes_per_frame = format.format.bytes_per_frame();
    if (channels == 0
        || bytes_per_sample == 0
        || bytes.size() != static_cast<size_t>(frame_count) * bytes_per_frame) {
        return false;
    }

    const auto sample_rate = static_cast<double>(format.format.sample_rate);
    for (core::contract::FrameCount frame = 0; frame < frame_count; ++frame) {
        const auto phase = 2.0 * std::numbers::pi
            * frequency_hz
            * static_cast<double>(first_frame + frame)
            / sample_rate;
        const auto value = amplitude * std::sin(phase);
        for (uint8_t channel = 0; channel < channels; ++channel) {
            auto* destination = bytes.data()
                + static_cast<size_t>(frame) * bytes_per_frame
                + static_cast<size_t>(channel) * bytes_per_sample;
            switch (format.format.sample_format) {
            case SampleFormat::Float32:
                store_sample(destination, static_cast<float>(value));
                break;
            case SampleFormat::Float64:
                store_sample(destination, value);
                break;
            case SampleFormat::Int16:
                store_sample(destination, static_cast<int16_t>(value * 32767.0));
                break;
            case SampleFormat::Int24:
                store_sample(
                    destination,
                    static_cast<int32_t>(value * 8388607.0) * 256);
                break;
            case SampleFormat::Int32:
                store_sample(
                    destination,
                    static_cast<int32_t>(value * 2147483647.0));
                break;
            default:
                return false;
            }
        }
    }
    return true;
}

} // namespace kivo::hardware_test::wasapi
