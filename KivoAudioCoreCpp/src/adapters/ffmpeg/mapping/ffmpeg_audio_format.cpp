#include "adapters/ffmpeg/mapping/ffmpeg_audio_format.hpp"

#include <cstring>
#include <limits>
#include <string_view>

namespace kivo::adapters::ffmpeg::detail {

namespace {

[[nodiscard]] core::contract::ChannelLayout map_channel_layout(
    int channel_count) noexcept {
    using core::contract::ChannelLayout;
    switch (channel_count) {
    case 1: return ChannelLayout::Mono;
    case 2: return ChannelLayout::Stereo;
    case 6: return ChannelLayout::Surround51;
    case 8: return ChannelLayout::Surround71;
    default: return ChannelLayout::Unknown;
    }
}

[[nodiscard]] core::contract::SampleFormat map_sample_format(
    AVSampleFormat sample_format,
    int bits_per_raw_sample) noexcept {
    using core::contract::SampleFormat;
    switch (av_get_packed_sample_fmt(sample_format)) {
    case AV_SAMPLE_FMT_S16:
        return SampleFormat::Int16;
    case AV_SAMPLE_FMT_S32:
        return bits_per_raw_sample == 24
            ? SampleFormat::Int24
            : SampleFormat::Int32;
    case AV_SAMPLE_FMT_FLT:
        return SampleFormat::Float32;
    case AV_SAMPLE_FMT_DBL:
        return SampleFormat::Float64;
    default:
        return SampleFormat::Unknown;
    }
}

[[nodiscard]] AVSampleFormat map_sample_format(
    core::contract::SampleFormat sample_format) noexcept {
    using core::contract::SampleFormat;
    switch (sample_format) {
    case SampleFormat::Int16: return AV_SAMPLE_FMT_S16;
    case SampleFormat::Int32: return AV_SAMPLE_FMT_S32;
    case SampleFormat::Float32: return AV_SAMPLE_FMT_FLT;
    case SampleFormat::Float64: return AV_SAMPLE_FMT_DBL;
    case SampleFormat::Int24:
    default: return AV_SAMPLE_FMT_NONE;
    }
}

[[nodiscard]] bool contains_name(
    const char* names,
    std::string_view expected) noexcept {
    if (names == nullptr) {
        return false;
    }
    const std::string_view value{names};
    size_t begin = 0;
    while (begin <= value.size()) {
        const auto end = value.find(',', begin);
        const auto item = value.substr(
            begin,
            end == std::string_view::npos ? value.size() - begin : end - begin);
        if (item == expected) {
            return true;
        }
        if (end == std::string_view::npos) {
            break;
        }
        begin = end + 1;
    }
    return false;
}

} // namespace

std::optional<core::contract::NativeDecodedFormat> map_native_format(
    AVSampleFormat sample_format,
    const AVChannelLayout& channel_layout,
    int sample_rate,
    int bits_per_raw_sample) noexcept {
    if (sample_rate <= 0
        || static_cast<uint64_t>(sample_rate)
            > std::numeric_limits<uint32_t>::max()) {
        return std::nullopt;
    }
    const auto mapped_sample =
        map_sample_format(sample_format, bits_per_raw_sample);
    const auto mapped_channels = map_channel_layout(channel_layout.nb_channels);
    if (mapped_sample == core::contract::SampleFormat::Unknown
        || mapped_channels == core::contract::ChannelLayout::Unknown) {
        return std::nullopt;
    }
    const auto valid_bits =
        core::contract::sample_format_valid_bits(mapped_sample);
    const auto container_bits =
        core::contract::sample_format_container_bits(mapped_sample);
    core::contract::NativeDecodedFormat result{{
        mapped_sample,
        mapped_channels,
        av_sample_fmt_is_planar(sample_format) != 0
            ? core::contract::FrameLayout::Planar
            : core::contract::FrameLayout::Interleaved,
        {},
        static_cast<uint32_t>(sample_rate),
        valid_bits,
        container_bits
    }};
    return result.is_valid()
        ? std::optional<core::contract::NativeDecodedFormat>{result}
        : std::nullopt;
}

std::optional<FfmpegTargetFormat> map_target_format(
    const core::contract::RenderFormat& format) noexcept {
    if (!format.is_valid()
        || format.format.frame_layout
            != core::contract::FrameLayout::Interleaved) {
        return std::nullopt;
    }
    FfmpegTargetFormat result{};
    result.sample_format = map_sample_format(format.format.sample_format);
    result.sample_rate = static_cast<int>(format.format.sample_rate);
    if (result.sample_format == AV_SAMPLE_FMT_NONE
        || result.sample_rate <= 0) {
        return std::nullopt;
    }
    av_channel_layout_default(
        &result.channel_layout,
        format.format.channel_count());
    if (result.channel_layout.nb_channels
        != format.format.channel_count()) {
        av_channel_layout_uninit(&result.channel_layout);
        return std::nullopt;
    }
    return result;
}

void uninit_target_format(FfmpegTargetFormat& format) noexcept {
    av_channel_layout_uninit(&format.channel_layout);
    format.sample_format = AV_SAMPLE_FMT_NONE;
    format.sample_rate = 0;
}

core::decode::AudioCodec map_audio_codec(AVCodecID codec_id) noexcept {
    using core::decode::AudioCodec;
    switch (codec_id) {
    case AV_CODEC_ID_PCM_S16LE:
    case AV_CODEC_ID_PCM_S16BE:
    case AV_CODEC_ID_PCM_S24LE:
    case AV_CODEC_ID_PCM_S24BE:
    case AV_CODEC_ID_PCM_S32LE:
    case AV_CODEC_ID_PCM_S32BE:
    case AV_CODEC_ID_PCM_F32LE:
    case AV_CODEC_ID_PCM_F32BE:
    case AV_CODEC_ID_PCM_F64LE:
    case AV_CODEC_ID_PCM_F64BE:
        return AudioCodec::Pcm;
    case AV_CODEC_ID_FLAC:
        return AudioCodec::Flac;
    case AV_CODEC_ID_MP3:
        return AudioCodec::Mp3;
    case AV_CODEC_ID_AAC:
        return AudioCodec::Aac;
    case AV_CODEC_ID_VORBIS:
        return AudioCodec::Vorbis;
    case AV_CODEC_ID_OPUS:
        return AudioCodec::Opus;
    case AV_CODEC_ID_DSD_LSBF:
    case AV_CODEC_ID_DSD_MSBF:
    case AV_CODEC_ID_DSD_LSBF_PLANAR:
    case AV_CODEC_ID_DSD_MSBF_PLANAR:
        return AudioCodec::Dsd;
    default:
        return AudioCodec::Unknown;
    }
}

core::decode::MediaContainer map_media_container(
    const AVInputFormat* input_format) noexcept {
    using core::decode::MediaContainer;
    if (input_format == nullptr) {
        return MediaContainer::Unknown;
    }
    const auto* names = input_format->name;
    if (contains_name(names, "wav")) return MediaContainer::Wave;
    if (contains_name(names, "flac")) return MediaContainer::Flac;
    if (contains_name(names, "mp3")) return MediaContainer::Mp3;
    if (contains_name(names, "mov")
        || contains_name(names, "mp4")
        || contains_name(names, "m4a")
        || contains_name(names, "3gp")
        || contains_name(names, "3g2")
        || contains_name(names, "mj2")) {
        return MediaContainer::Mp4;
    }
    if (contains_name(names, "ogg")) return MediaContainer::Ogg;
    if (contains_name(names, "matroska")
        || contains_name(names, "webm")) {
        return MediaContainer::Matroska;
    }
    if (contains_name(names, "dsf")) return MediaContainer::Dsf;
    if (contains_name(names, "dsdiff")) return MediaContainer::Dsdiff;
    return MediaContainer::Unknown;
}

} // namespace kivo::adapters::ffmpeg::detail
