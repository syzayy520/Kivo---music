#pragma once

#include <optional>

extern "C" {
#include <libavcodec/codec_par.h>
#include <libavformat/avformat.h>
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
}

#include "kivo/core/contract/format/roles/native_decoded_format.hpp"
#include "kivo/core/contract/format/roles/render_format.hpp"
#include "kivo/core/decode/media/audio_codec.hpp"
#include "kivo/core/decode/media/media_container.hpp"

namespace kivo::adapters::ffmpeg::detail {

struct FfmpegTargetFormat {
    AVSampleFormat sample_format{AV_SAMPLE_FMT_NONE};
    AVChannelLayout channel_layout{};
    int sample_rate{0};
};

[[nodiscard]] std::optional<core::contract::NativeDecodedFormat>
map_native_format(
    AVSampleFormat sample_format,
    const AVChannelLayout& channel_layout,
    int sample_rate,
    int bits_per_raw_sample) noexcept;

[[nodiscard]] std::optional<FfmpegTargetFormat> map_target_format(
    const core::contract::RenderFormat& format) noexcept;

void uninit_target_format(FfmpegTargetFormat& format) noexcept;

[[nodiscard]] core::decode::AudioCodec map_audio_codec(
    AVCodecID codec_id) noexcept;
[[nodiscard]] core::decode::MediaContainer map_media_container(
    const AVInputFormat* input_format) noexcept;

} // namespace kivo::adapters::ffmpeg::detail
