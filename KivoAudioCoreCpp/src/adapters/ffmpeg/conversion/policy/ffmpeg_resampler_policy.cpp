#include "adapters/ffmpeg/conversion/policy/ffmpeg_resampler_policy.hpp"

#include <cstdint>

extern "C" {
#include <libavutil/opt.h>
}

namespace kivo::adapters::ffmpeg::detail {

namespace {

struct ResamplerOptions {
    int64_t filter_size;
    int64_t phase_shift;
    double cutoff;
};

[[nodiscard]] constexpr ResamplerOptions options(
    core::processing::ResampleQuality quality) noexcept {
    using core::processing::ResampleQuality;
    switch (quality) {
    case ResampleQuality::Fast:
        return {16, 8, 0.80};
    case ResampleQuality::High:
        return {64, 10, 0.98};
    case ResampleQuality::Balanced:
    default:
        return {32, 10, 0.95};
    }
}

[[nodiscard]] constexpr bool precision_reduction(
    const core::contract::AudioFormatDescriptor& source,
    const core::contract::AudioFormatDescriptor& target) noexcept {
    return core::contract::sample_format_is_integer(target.sample_format)
        && (core::contract::sample_format_is_float(source.sample_format)
            || source.bits_per_sample > target.bits_per_sample);
}

} // namespace

bool configure_ffmpeg_resampler(
    SwrContext& context,
    const core::contract::AudioFormatDescriptor& source,
    const core::contract::AudioFormatDescriptor& target,
    const core::contract::ConversionPolicy& conversion,
    core::processing::ResampleQuality resample_quality,
    core::processing::DitherPolicy dither_policy,
    core::processing::AudioConversionSnapshot& snapshot) noexcept {
    snapshot = {};
    snapshot.conversion = conversion;
    snapshot.resample_quality = resample_quality;
    snapshot.dither_policy = dither_policy;
    snapshot.dither_active =
        dither_policy == core::processing::DitherPolicy::TriangularPdf
        && precision_reduction(source, target);

    if (conversion.sample_rate_convert) {
        const auto values = options(resample_quality);
        if (av_opt_set_int(
                &context, "filter_size", values.filter_size, 0) < 0
            || av_opt_set_int(
                &context, "phase_shift", values.phase_shift, 0) < 0
            || av_opt_set_double(
                &context, "cutoff", values.cutoff, 0) < 0) {
            return false;
        }
    }
    return !snapshot.dither_active
        || av_opt_set_int(
            &context,
            "dither_method",
            SWR_DITHER_TRIANGULAR,
            0) >= 0;
}

} // namespace kivo::adapters::ffmpeg::detail
