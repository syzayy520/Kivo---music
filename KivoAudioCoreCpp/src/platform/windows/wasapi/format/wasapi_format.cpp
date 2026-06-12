#include "platform/windows/wasapi/format/wasapi_format.hpp"

#include <ks.h>
#include <ksmedia.h>

#include <limits>

namespace kivo::platform::windows::wasapi::detail {

namespace {

using core::contract::ChannelLayout;
using core::contract::ChannelMask;
using core::contract::SampleFormat;

[[nodiscard]] ChannelLayout channel_layout(uint16_t channels) noexcept {
    switch (channels) {
    case 1: return ChannelLayout::Mono;
    case 2: return ChannelLayout::Stereo;
    case 6: return ChannelLayout::Surround51;
    case 8: return ChannelLayout::Surround71;
    default: return ChannelLayout::Unknown;
    }
}

[[nodiscard]] SampleFormat sample_format(
    bool floating_point,
    uint16_t valid_bits,
    uint16_t container_bits) noexcept {
    if (floating_point) {
        if (valid_bits == 32 && container_bits == 32) {
            return SampleFormat::Float32;
        }
        if (valid_bits == 64 && container_bits == 64) {
            return SampleFormat::Float64;
        }
        return SampleFormat::Unknown;
    }

    if (valid_bits == 16 && container_bits == 16) {
        return SampleFormat::Int16;
    }
    if (valid_bits == 24 && container_bits == 32) {
        return SampleFormat::Int24;
    }
    if (valid_bits == 32 && container_bits == 32) {
        return SampleFormat::Int32;
    }
    return SampleFormat::Unknown;
}

} // namespace

bool map_wave_format(
    const WAVEFORMATEX* wave_format,
    core::contract::RenderFormat& render_format) noexcept {
    render_format = {};
    if (wave_format == nullptr
        || wave_format->nChannels == 0
        || wave_format->nSamplesPerSec == 0
        || wave_format->wBitsPerSample == 0) {
        return false;
    }

    bool floating_point = false;
    uint16_t valid_bits = wave_format->wBitsPerSample;
    uint32_t channel_mask = 0;

    if (wave_format->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
        if (wave_format->cbSize < sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX)) {
            return false;
        }
        const auto* extensible =
            reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(wave_format);
        if (IsEqualGUID(extensible->SubFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)) {
            floating_point = true;
        } else if (!IsEqualGUID(extensible->SubFormat, KSDATAFORMAT_SUBTYPE_PCM)) {
            return false;
        }
        valid_bits = extensible->Samples.wValidBitsPerSample;
        channel_mask = extensible->dwChannelMask;
    } else if (wave_format->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) {
        floating_point = true;
    } else if (wave_format->wFormatTag != WAVE_FORMAT_PCM) {
        return false;
    }

    const auto layout = channel_layout(wave_format->nChannels);
    const auto format =
        sample_format(floating_point, valid_bits, wave_format->wBitsPerSample);
    if (layout == ChannelLayout::Unknown || format == SampleFormat::Unknown) {
        return false;
    }

    const auto bytes_per_sample = wave_format->wBitsPerSample / 8u;
    if ((wave_format->wBitsPerSample % 8u) != 0
        || wave_format->nBlockAlign != wave_format->nChannels * bytes_per_sample) {
        return false;
    }
    const auto expected_average_bytes =
        static_cast<uint64_t>(wave_format->nSamplesPerSec) * wave_format->nBlockAlign;
    if (expected_average_bytes > std::numeric_limits<uint32_t>::max()
        || wave_format->nAvgBytesPerSec != expected_average_bytes) {
        return false;
    }

    core::contract::AudioFormatDescriptor descriptor{};
    descriptor.sample_format = format;
    descriptor.channel_layout = layout;
    descriptor.frame_layout = core::contract::FrameLayout::Interleaved;
    descriptor.channel_mask = ChannelMask{channel_mask};
    descriptor.sample_rate = wave_format->nSamplesPerSec;
    descriptor.bits_per_sample = static_cast<uint8_t>(valid_bits);
    descriptor.container_bits_per_sample =
        static_cast<uint8_t>(wave_format->wBitsPerSample);
    if (!descriptor.is_valid()) {
        return false;
    }

    render_format = {descriptor};
    return true;
}

} // namespace kivo::platform::windows::wasapi::detail
