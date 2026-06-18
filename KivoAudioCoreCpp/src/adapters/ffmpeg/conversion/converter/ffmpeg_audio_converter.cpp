#include "adapters/ffmpeg/conversion/converter/ffmpeg_audio_converter.hpp"

#include "adapters/ffmpeg/conversion/policy/ffmpeg_resampler_policy.hpp"
#include "adapters/ffmpeg/conversion/truth/ffmpeg_conversion_delay.hpp"

namespace kivo::adapters::ffmpeg::detail {

FfmpegAudioConverter::~FfmpegAudioConverter() {
    close();
}

bool FfmpegAudioConverter::open(
    const AVFrame& frame,
    const core::contract::AudioFormatDescriptor& source,
    const core::contract::RenderFormat& target,
    const core::contract::ConversionPolicy& conversion,
    core::processing::ResampleQuality resample_quality,
    core::processing::DitherPolicy dither_policy) noexcept {
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
        || !configure_ffmpeg_resampler(
            *context_,
            source,
            target.format,
            conversion,
            resample_quality,
            dither_policy,
            snapshot_)
        || swr_init(context_) < 0) {
        failure_ = core::decode::DecodeFailure::ConversionFailed;
        close();
        return false;
    }
    refresh_ffmpeg_conversion_delay(
        *context_,
        render_format_.format.sample_rate,
        snapshot_);

    // Pre-reserve output buffer based on first frame size + 2× resampler
    // headroom, so convert_samples() never heap-allocates in steady state.
    const auto bytes_per_frame = render_format_.format.bytes_per_frame();
    if (bytes_per_frame > 0 && frame.nb_samples > 0) {
        const auto estimated = swr_get_out_samples(context_, frame.nb_samples);
        if (estimated > 0) {
            try {
                output_.reserve(
                    static_cast<size_t>(estimated) * bytes_per_frame * 2);
            } catch (...) {}
        }
    }

    failure_ = core::decode::DecodeFailure::None;
    return true;
}

void FfmpegAudioConverter::close() noexcept {
    swr_free(&context_);
    uninit_target_format(target_);
    render_format_ = {};
    snapshot_ = {};
    output_.clear();
    frame_count_ = 0;
}

bool FfmpegAudioConverter::is_open() const noexcept {
    return context_ != nullptr;
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

core::processing::AudioConversionSnapshot
FfmpegAudioConverter::snapshot() const noexcept {
    return snapshot_;
}

} // namespace kivo::adapters::ffmpeg::detail
