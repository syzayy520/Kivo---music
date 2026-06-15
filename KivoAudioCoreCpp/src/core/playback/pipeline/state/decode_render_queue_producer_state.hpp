#pragma once

#include <array>
#include <optional>

#include "../buffer/decode_render_queue_buffer.hpp"
#include "kivo/core/playback/pipeline/producer/decode_render_queue_producer.hpp"
#include "kivo/core/processing/chain/audio_processing_chain.hpp"

namespace kivo::core::playback {

class DecodeRenderQueueProducer::Impl {
public:
    Impl(
        decode::AudioDecodeBoundary& decoder,
        render::SpscAudioBlockQueue& queue,
        contract::RenderFormat format,
        render::RenderGenerationSet generations,
        decode::DecodeGeneration decode_generation,
        DecodeRenderQueueProducerConfiguration configuration) noexcept;

    [[nodiscard]] bool allocate() noexcept;
    [[nodiscard]] DecodeRenderQueueProducerResult step() noexcept;
    [[nodiscard]] DecodeRenderQueueProducerResult prefetch() noexcept;
    [[nodiscard]] DecodeRenderQueueProducerSnapshot snapshot() const noexcept;

private:
    [[nodiscard]] pipeline_buffer::StoreResult store_decoded(
        const decode::DecodedAudioBlockView& block) noexcept;
    void activate_held(bool terminal) noexcept;
    [[nodiscard]] DecodeRenderQueueProducerResult enqueue_active() noexcept;
    [[nodiscard]] DecodeRenderQueueProducerResult failed(
        decode::DecodeFailure failure) noexcept;
    [[nodiscard]] DecodeRenderQueueProducerResult finish() noexcept;
    static void saturating_increment(uint64_t& value) noexcept;
    static void saturating_add(
        contract::FrameCount& value,
        contract::FrameCount amount) noexcept;

    decode::AudioDecodeBoundary& decoder_;
    render::SpscAudioBlockQueue& queue_;
    contract::RenderFormat format_{};
    render::RenderGenerationSet generations_{};
    decode::DecodeGeneration decode_generation_{};
    DecodeRenderQueueProducerConfiguration configuration_{};
    std::unique_ptr<processing::AudioProcessingChain> processing_{};
    std::array<pipeline_buffer::BufferedBlock, 2> blocks_{};
    std::optional<size_t> held_index_{};
    std::optional<size_t> active_index_{};
    size_t stored_index_{0};
    contract::FrameCount active_cursor_{0};
    bool held_is_terminal_{false};
    bool active_is_terminal_{false};
    contract::BufferId next_buffer_id_{};
    DecodeRenderQueueProducerSnapshot snapshot_{};
};

} // namespace kivo::core::playback
