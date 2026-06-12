#pragma once

#include "kivo/core/decode/generation/decode_generation.hpp"
#include "kivo/core/playback/pipeline/configuration/decode_render_queue_producer_configuration.hpp"
#include "kivo/core/render/format/render_open_request.hpp"
#include "kivo/core/render/lifecycle/render_control_request.hpp"
#include "kivo/core/render/queue/spsc_audio_block_queue_configuration.hpp"
#include "kivo/core/processing/policy/dither_policy.hpp"
#include "kivo/core/processing/policy/resample_quality.hpp"

namespace kivo::core::playback {

struct PlaybackRuntimeOpenRequest {
    render::RenderOpenRequest render_request{};
    render::SpscAudioBlockQueueConfiguration queue_configuration{};
    DecodeRenderQueueProducerConfiguration producer_configuration{};
    decode::DecodeGeneration decode_generation{};
    render::RenderDrainRequest drain_request{
        std::chrono::seconds{2}};
    bool allow_conversion{false};
    processing::ResampleQuality resample_quality{
        processing::ResampleQuality::Balanced};
    processing::DitherPolicy conversion_dither{
        processing::DitherPolicy::Disabled};

    [[nodiscard]] bool is_valid() const noexcept {
        return render_request.is_valid()
            && queue_configuration.is_valid()
            && producer_configuration.is_valid()
            && decode_generation.value() != 0
            && drain_request.is_valid()
            && (!producer_configuration.processing.bit_perfect_required
                || (!allow_conversion
                    && conversion_dither
                        == processing::DitherPolicy::Disabled));
    }

    [[nodiscard]] bool operator==(
        const PlaybackRuntimeOpenRequest&) const noexcept = default;
};

} // namespace kivo::core::playback
