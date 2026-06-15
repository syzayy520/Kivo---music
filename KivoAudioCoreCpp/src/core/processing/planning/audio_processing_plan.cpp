#include "kivo/core/processing/result/audio_processing_plan.hpp"

#include <algorithm>
#include <cmath>

namespace kivo::core::processing {

namespace {

constexpr double kUnityTolerance = 1.0e-12;

[[nodiscard]] bool differs_from_unity(double value) noexcept {
    return std::abs(value - 1.0) > kUnityTolerance;
}

} // namespace

AudioProcessingPlan build_audio_processing_plan(
    contract::RenderFormat format,
    const AudioProcessingConfiguration& configuration) noexcept {
    AudioProcessingPlan plan{};
    plan.format = format;
    plan.dither_seed = configuration.dither_seed;
    if (!configuration.is_valid()) {
        plan.failure = AudioProcessingFailure::InvalidConfiguration;
        return plan;
    }
    if (!format.is_valid()
        || format.format.frame_layout != contract::FrameLayout::Interleaved) {
        plan.failure = AudioProcessingFailure::UnsupportedFormat;
        return plan;
    }
    if (configuration.bit_perfect_required
        && configuration.mutation_requested()) {
        plan.failure = AudioProcessingFailure::BitPerfectConflict;
        return plan;
    }

    plan.replay_gain_linear = configuration.replay_gain.enabled
        ? std::pow(10.0, configuration.replay_gain.gain_db / 20.0)
        : 1.0;
    if (configuration.replay_gain.enabled
        && configuration.replay_gain.prevent_clipping
        && configuration.replay_gain.peak.has_value()) {
        plan.replay_gain_linear = std::min(
            plan.replay_gain_linear,
            1.0 / *configuration.replay_gain.peak);
    }
    plan.volume_linear = configuration.volume.enabled
        ? configuration.volume.linear_gain
        : 1.0;
    plan.effective_gain =
        plan.replay_gain_linear * plan.volume_linear;
    plan.participation.replay_gain_active =
        configuration.replay_gain.enabled
        && differs_from_unity(plan.replay_gain_linear);
    plan.participation.volume_active =
        configuration.volume.enabled
        && differs_from_unity(plan.volume_linear);
    plan.strict_bypass =
        !plan.participation.any_processing_active();
    plan.dither_active =
        !plan.strict_bypass
        && contract::sample_format_is_integer(format.format.sample_format)
        && configuration.dither_policy == DitherPolicy::TriangularPdf;
    return plan;
}

} // namespace kivo::core::processing
