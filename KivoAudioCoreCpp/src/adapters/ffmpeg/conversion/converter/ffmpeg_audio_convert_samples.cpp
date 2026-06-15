#include "adapters/ffmpeg/conversion/converter/ffmpeg_audio_converter.hpp"

#include <cstdint>
#include <limits>

#include "adapters/ffmpeg/conversion/truth/ffmpeg_conversion_delay.hpp"

namespace kivo::adapters::ffmpeg::detail {

bool FfmpegAudioConverter::convert(const AVFrame& frame) noexcept {
    if (context_ == nullptr || frame.nb_samples <= 0) {
        failure_ = core::decode::DecodeFailure::InvalidMediaData;
        return false;
    }
    return convert_samples(frame.extended_data, frame.nb_samples);
}

bool FfmpegAudioConverter::drain() noexcept {
    if (context_ == nullptr) {
        failure_ = core::decode::DecodeFailure::BoundaryFailure;
        return false;
    }
    return convert_samples(nullptr, 0);
}

bool FfmpegAudioConverter::convert_samples(
    const uint8_t* const* input,
    int input_frames) noexcept {
    frame_count_ = 0;
    const auto maximum_frames = swr_get_out_samples(context_, input_frames);
    const auto bytes_per_frame = render_format_.format.bytes_per_frame();
    if (maximum_frames < 0
        || bytes_per_frame == 0
        || static_cast<uint64_t>(maximum_frames)
            > std::numeric_limits<size_t>::max() / bytes_per_frame) {
        failure_ = core::decode::DecodeFailure::ConversionFailed;
        return false;
    }

    try {
        output_.resize(
            static_cast<size_t>(maximum_frames) * bytes_per_frame);
    } catch (...) {
        failure_ = core::decode::DecodeFailure::BoundaryFailure;
        return false;
    }
    auto* output = reinterpret_cast<uint8_t*>(output_.data());
    uint8_t* output_planes[] = {output};
    const auto converted = swr_convert(
        context_,
        output_planes,
        maximum_frames,
        input,
        input_frames);
    if (converted < 0) {
        failure_ = core::decode::DecodeFailure::ConversionFailed;
        output_.clear();
        return false;
    }
    output_.resize(static_cast<size_t>(converted) * bytes_per_frame);
    frame_count_ = static_cast<core::contract::FrameCount>(converted);
    refresh_ffmpeg_conversion_delay(
        *context_,
        render_format_.format.sample_rate,
        snapshot_);
    failure_ = core::decode::DecodeFailure::None;
    return true;
}

} // namespace kivo::adapters::ffmpeg::detail
