#pragma once

#include <memory>

#include "kivo/core/contract/format/roles/render_format.hpp"
#include "kivo/core/decode/boundary/audio_decode_boundary.hpp"
#include "kivo/core/playback/pipeline/configuration/decode_render_queue_producer_configuration.hpp"
#include "kivo/core/playback/pipeline/result/decode_render_queue_producer_result.hpp"
#include "kivo/core/playback/pipeline/snapshot/decode_render_queue_producer_snapshot.hpp"
#include "kivo/core/render/generation/render_generation_set.hpp"
#include "kivo/core/render/queue/spsc_audio_block_queue.hpp"

namespace kivo::core::playback {

class DecodeRenderQueueProducer final {
public:
    [[nodiscard]] static std::unique_ptr<DecodeRenderQueueProducer> create(
        decode::AudioDecodeBoundary& decoder,
        render::SpscAudioBlockQueue& queue,
        contract::RenderFormat format,
        render::RenderGenerationSet generations,
        decode::DecodeGeneration decode_generation,
        DecodeRenderQueueProducerConfiguration configuration) noexcept;

    ~DecodeRenderQueueProducer();

    DecodeRenderQueueProducer(const DecodeRenderQueueProducer&) = delete;
    DecodeRenderQueueProducer& operator=(const DecodeRenderQueueProducer&) = delete;
    DecodeRenderQueueProducer(DecodeRenderQueueProducer&&) = delete;
    DecodeRenderQueueProducer& operator=(DecodeRenderQueueProducer&&) = delete;

    [[nodiscard]] DecodeRenderQueueProducerResult step() noexcept;
    [[nodiscard]] DecodeRenderQueueProducerSnapshot snapshot() const noexcept;

private:
    class Impl;
    explicit DecodeRenderQueueProducer(std::unique_ptr<Impl> impl) noexcept;

    std::unique_ptr<Impl> impl_;
};

} // namespace kivo::core::playback
