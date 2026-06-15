#pragma once

#include "kivo/core/decode/boundary/audio_decode_boundary.hpp"
#include "kivo/core/playback/gapless/plan/gapless_track_plan.hpp"
#include "kivo/core/playback/pipeline/configuration/decode_render_queue_producer_configuration.hpp"
#include "kivo/core/render/generation/render_generation_set.hpp"

namespace kivo::core::playback {

struct GaplessTrackPrepareRequest {
    decode::AudioDecodeBoundary* decoder{nullptr};
    GaplessTrackPlan plan{};
    render::RenderGenerationSet generations{};
    decode::DecodeGeneration decode_generation{};
    DecodeRenderQueueProducerConfiguration producer_configuration{};

    [[nodiscard]] bool is_valid() const noexcept {
        return decoder != nullptr
            && plan.is_valid()
            && decode_generation.value() != 0
            && producer_configuration.is_valid();
    }
};

} // namespace kivo::core::playback
