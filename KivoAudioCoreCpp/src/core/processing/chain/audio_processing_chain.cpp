#include "kivo/core/processing/chain/audio_processing_chain.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <new>

#include "core/processing/gain/audio_gain_application.hpp"

namespace kivo::core::processing {

AudioProcessingChain::AudioProcessingChain(
    AudioProcessingPlan plan,
    AudioProcessingConfiguration configuration) noexcept
    : plan_(plan),
      configuration_(configuration),
      dither_state_(plan.dither_seed) {
    snapshot_.participation = plan.participation;
    snapshot_.replay_gain_linear = plan.replay_gain_linear;
    snapshot_.volume_linear = plan.volume_linear;
    snapshot_.effective_gain = plan.effective_gain;
    snapshot_.dither_active = plan.dither_active;
    snapshot_.strict_bypass = plan.strict_bypass;
}

std::unique_ptr<AudioProcessingChain> AudioProcessingChain::create(
    contract::RenderFormat format,
    AudioProcessingConfiguration configuration) noexcept {
    auto plan = build_audio_processing_plan(format, configuration);
    return plan.is_valid()
        ? std::unique_ptr<AudioProcessingChain>{
            new (std::nothrow) AudioProcessingChain(plan, configuration)}
        : nullptr;
}

bool AudioProcessingChain::set_volume(double volume) noexcept {
    if (!std::isfinite(volume)) {
        return false;
    }
    AudioProcessingConfiguration updated = configuration_;
    updated.volume.enabled = true;
    updated.volume.linear_gain = std::clamp(volume, 0.0, 1.0);
    const auto rebuilt = build_audio_processing_plan(plan_.format, updated);
    if (!rebuilt.is_valid()) {
        return false;  // e.g. non-unity volume under bit-perfect — leave as-is
    }
    // dither_seed and format are carried through `rebuilt`; dither_state_ (the
    // running PRNG) is intentionally preserved for continuous dither.
    configuration_ = updated;
    plan_ = rebuilt;
    snapshot_.participation = plan_.participation;
    snapshot_.replay_gain_linear = plan_.replay_gain_linear;
    snapshot_.volume_linear = plan_.volume_linear;
    snapshot_.effective_gain = plan_.effective_gain;
    snapshot_.dither_active = plan_.dither_active;
    snapshot_.strict_bypass = plan_.strict_bypass;
    return true;
}

AudioProcessingResult AudioProcessingChain::process(
    std::span<std::byte> bytes,
    contract::FrameCount frame_count) noexcept {
    const auto bytes_per_frame = plan_.format.format.bytes_per_frame();
    if (!plan_.is_valid()
        || frame_count == 0
        || bytes_per_frame == 0
        || frame_count > std::numeric_limits<size_t>::max() / bytes_per_frame
        || bytes.size() != static_cast<size_t>(frame_count) * bytes_per_frame) {
        return {
            AudioProcessingDisposition::Failed,
            AudioProcessingFailure::InvalidBuffer,
            0,
            0};
    }
    if (plan_.strict_bypass) {
        return {
            AudioProcessingDisposition::Bypassed,
            AudioProcessingFailure::None,
            frame_count,
            0};
    }

    const auto applied = detail::apply_gain(
        bytes,
        plan_.format,
        frame_count,
        plan_.effective_gain,
        plan_.dither_active,
        dither_state_);
    if (!applied.succeeded) {
        return {
            AudioProcessingDisposition::Failed,
            AudioProcessingFailure::UnsupportedFormat,
            0,
            0};
    }
    snapshot_.processed_frames =
        frame_count > std::numeric_limits<contract::FrameCount>::max()
                - snapshot_.processed_frames
            ? std::numeric_limits<contract::FrameCount>::max()
            : snapshot_.processed_frames + frame_count;
    if (snapshot_.processed_blocks != std::numeric_limits<uint64_t>::max()) {
        ++snapshot_.processed_blocks;
    }
    snapshot_.clipped_samples =
        applied.clipped_samples
            > std::numeric_limits<uint64_t>::max()
                - snapshot_.clipped_samples
        ? std::numeric_limits<uint64_t>::max()
        : snapshot_.clipped_samples + applied.clipped_samples;
    return {
        AudioProcessingDisposition::Processed,
        AudioProcessingFailure::None,
        frame_count,
        applied.clipped_samples};
}

AudioProcessingSnapshot AudioProcessingChain::snapshot() const noexcept {
    return snapshot_;
}

} // namespace kivo::core::processing
