#pragma once

#include "kivo/core/decode/generation/decode_generation.hpp"
#include "kivo/core/playback/pipeline/decode_render_queue_producer_configuration.hpp"
#include "kivo/core/render/format/render_open_request.hpp"
#include "kivo/core/render/queue/spsc_audio_block_queue_configuration.hpp"

namespace kivo::core::playback {

struct PlaybackRuntimeOpenRequest {
    render::RenderOpenRequest render_request{};
    render::SpscAudioBlockQueueConfiguration queue_configuration{};
    DecodeRenderQueueProducerConfiguration producer_configuration{};
    decode::DecodeGeneration decode_generation{};
    bool allow_conversion{false};

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return render_request.is_valid()
            && queue_configuration.is_valid()
            && producer_configuration.is_valid()
            && decode_generation.value() != 0;
    }

    [[nodiscard]] constexpr bool operator==(
        const PlaybackRuntimeOpenRequest&) const noexcept = default;
};

} // namespace kivo::core::playback
