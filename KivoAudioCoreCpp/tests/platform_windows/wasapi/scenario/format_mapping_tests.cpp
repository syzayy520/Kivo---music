#include "../fixture/wasapi_test_runner.hpp"

#include <Windows.h>
#include <ks.h>
#include <ksmedia.h>
#include <mmreg.h>

#include "kivo/platform/windows/wasapi/renderer/wasapi_renderer.hpp"
#include "platform/windows/wasapi/format/wasapi_format.hpp"

namespace {

using namespace kivo;

void pcm16_stereo_maps_to_core_format() {
    WAVEFORMATEX wave{};
    wave.wFormatTag = WAVE_FORMAT_PCM;
    wave.nChannels = 2;
    wave.nSamplesPerSec = 48000;
    wave.wBitsPerSample = 16;
    wave.nBlockAlign = 4;
    wave.nAvgBytesPerSec = wave.nSamplesPerSec * wave.nBlockAlign;

    core::contract::RenderFormat format{};
    WASAPI_ASSERT(platform::windows::wasapi::detail::map_wave_format(&wave, format));
    WASAPI_ASSERT(format.format.sample_format == core::contract::SampleFormat::Int16);
    WASAPI_ASSERT(format.format.channel_layout == core::contract::ChannelLayout::Stereo);
    WASAPI_ASSERT(format.format.sample_rate == 48000);
    WASAPI_ASSERT(format.format.bytes_per_frame() == 4);
}

void extensible_float_stereo_maps_to_core_format() {
    WAVEFORMATEXTENSIBLE wave{};
    wave.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
    wave.Format.nChannels = 2;
    wave.Format.nSamplesPerSec = 48000;
    wave.Format.wBitsPerSample = 32;
    wave.Format.nBlockAlign = 8;
    wave.Format.nAvgBytesPerSec = wave.Format.nSamplesPerSec * wave.Format.nBlockAlign;
    wave.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
    wave.Samples.wValidBitsPerSample = 32;
    wave.dwChannelMask = KSAUDIO_SPEAKER_STEREO;
    wave.SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;

    core::contract::RenderFormat format{};
    WASAPI_ASSERT(platform::windows::wasapi::detail::map_wave_format(
        &wave.Format,
        format));
    WASAPI_ASSERT(format.format.sample_format == core::contract::SampleFormat::Float32);
    WASAPI_ASSERT(format.format.effective_channel_mask() == core::contract::kStereoMask);
}

void extensible_24_in_32_maps_without_losing_container_truth() {
    WAVEFORMATEXTENSIBLE wave{};
    wave.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
    wave.Format.nChannels = 2;
    wave.Format.nSamplesPerSec = 96000;
    wave.Format.wBitsPerSample = 32;
    wave.Format.nBlockAlign = 8;
    wave.Format.nAvgBytesPerSec = wave.Format.nSamplesPerSec * wave.Format.nBlockAlign;
    wave.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
    wave.Samples.wValidBitsPerSample = 24;
    wave.dwChannelMask = KSAUDIO_SPEAKER_STEREO;
    wave.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;

    core::contract::RenderFormat format{};
    WASAPI_ASSERT(platform::windows::wasapi::detail::map_wave_format(
        &wave.Format,
        format));
    WASAPI_ASSERT(format.format.sample_format == core::contract::SampleFormat::Int24);
    WASAPI_ASSERT(format.format.valid_bits_per_sample() == 24);
    WASAPI_ASSERT(format.format.container_bits() == 32);
}

void packed_24_bit_pcm_is_rejected() {
    WAVEFORMATEX wave{};
    wave.wFormatTag = WAVE_FORMAT_PCM;
    wave.nChannels = 2;
    wave.nSamplesPerSec = 48000;
    wave.wBitsPerSample = 24;
    wave.nBlockAlign = 6;
    wave.nAvgBytesPerSec = wave.nSamplesPerSec * wave.nBlockAlign;

    core::contract::RenderFormat format{};
    WASAPI_ASSERT(!platform::windows::wasapi::detail::map_wave_format(&wave, format));
}

void unsupported_channel_layout_is_rejected() {
    WAVEFORMATEX wave{};
    wave.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
    wave.nChannels = 3;
    wave.nSamplesPerSec = 48000;
    wave.wBitsPerSample = 32;
    wave.nBlockAlign = 12;
    wave.nAvgBytesPerSec = wave.nSamplesPerSec * wave.nBlockAlign;

    core::contract::RenderFormat format{};
    WASAPI_ASSERT(!platform::windows::wasapi::detail::map_wave_format(&wave, format));
}

void inconsistent_average_byte_rate_is_rejected() {
    WAVEFORMATEX wave{};
    wave.wFormatTag = WAVE_FORMAT_PCM;
    wave.nChannels = 2;
    wave.nSamplesPerSec = 48000;
    wave.wBitsPerSample = 16;
    wave.nBlockAlign = 4;
    wave.nAvgBytesPerSec = 1;

    core::contract::RenderFormat format{};
    WASAPI_ASSERT(!platform::windows::wasapi::detail::map_wave_format(&wave, format));
}

void adapter_mode_capabilities_are_truthful() {
    constexpr auto modes =
        platform::windows::wasapi::WasapiRenderer::supported_modes();
    static_assert(modes.shared_mode);
    static_assert(!modes.exclusive_mode);
    static_assert(modes.event_driven_shared_mode);
    WASAPI_ASSERT(modes.shared_mode);
    WASAPI_ASSERT(!modes.exclusive_mode);
}

} // namespace

void run_wasapi_format_mapping_tests(WasapiTestRunner& runner) {
    runner.run("pcm16_stereo_maps_to_core_format", pcm16_stereo_maps_to_core_format);
    runner.run(
        "extensible_float_stereo_maps_to_core_format",
        extensible_float_stereo_maps_to_core_format);
    runner.run(
        "extensible_24_in_32_maps_without_losing_container_truth",
        extensible_24_in_32_maps_without_losing_container_truth);
    runner.run("packed_24_bit_pcm_is_rejected", packed_24_bit_pcm_is_rejected);
    runner.run(
        "unsupported_channel_layout_is_rejected",
        unsupported_channel_layout_is_rejected);
    runner.run(
        "inconsistent_average_byte_rate_is_rejected",
        inconsistent_average_byte_rate_is_rejected);
    runner.run(
        "adapter_mode_capabilities_are_truthful",
        adapter_mode_capabilities_are_truthful);
}
