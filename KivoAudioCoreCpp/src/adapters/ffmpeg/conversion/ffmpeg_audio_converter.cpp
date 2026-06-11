#include "conversion/ffmpeg_audio_converter.hpp"

#include <cstdint>
#include <limits>

extern "C" {
#include <libavutil/error.h>
}

namespace kivo::adapters::ffmpeg::detail {

FfmpegAudioConverter::~FfmpegAudioConverter() {
    close();
}

bool FfmpegAudioConverter::open(
    const AVFrame& frame,
    const core::contract::RenderFormat& target) noexcept {
    close();
    auto mapped_target = map_target_format(target);
    if (!mapped_target.has_value()) {
        failure_ = core::decode::DecodeFailure::ConversionRequired;
        return false;
    }
    target_ = *mapped_target;
    render_format_ = target;

    if (swr_alloc_set_opts2(
            &context_,
            &target_.channel_layout,
            target_.sample_format,
            target_.sample_rate,
            &frame.ch_layout,
            static_cast<AVSampleFormat>(frame.format),
            frame.sample_rate,
            0,
            nullptr) < 0
        || context_ == nullptr
        || swr_init(context_) < 0) {
        failure_ = core::decode::DecodeFailure::ConversionFailed;
        close();
        return false;
    }
    failure_ = core::decode::DecodeFailure::None;
    return true;
}

void FfmpegAudioConverter::close() noexcept {
    swr_free(&context_);
    uninit_target_format(target_);
    render_format_ = {};
    output_.clear();
    frame_count_ = 0;
}

bool FfmpegAudioConverter::is_open() const noexcept {
    return context_ != nullptr;
}

bool FfmpegAudioConverter::convert(const AVFrame& frame) noexcept {
    if (context_ == nullptr || frame.nb_samples <= 0) {
        failure_ = core::decode::DecodeFailure::InvalidMediaData;
        return false;
    }
    const uint8_t* const* input = frame.extended_data;
    return convert_samples(input, frame.nb_samples);
}

bool FfmpegAudioConverter::drain() noexcept {
    if (context_ == nullptr) {
        failure_ = core::decode::DecodeFailure::BoundaryFailure;
        return false;
    }
    return convert_samples(nullptr, 0);
}

std::span<const std::byte> FfmpegAudioConverter::bytes() const noexcept {
    return output_;
}

core::contract::FrameCount FfmpegAudioConverter::frame_count() const noexcept {
    return frame_count_;
}

core::decode::DecodeFailure FfmpegAudioConverter::failure() const noexcept {
    return failure_;
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
    failure_ = core::decode::DecodeFailure::None;
    return true;
}

} // namespace kivo::adapters::ffmpeg::detail
