#include "../state/decode_render_queue_producer_state.hpp"

#include <limits>
#include <new>
#include <utility>

namespace kivo::core::playback {

DecodeRenderQueueProducer::Impl::Impl(
    decode::AudioDecodeBoundary& decoder,
    render::SpscAudioBlockQueue& queue,
    contract::RenderFormat format,
    render::RenderGenerationSet generations,
    decode::DecodeGeneration decode_generation,
    DecodeRenderQueueProducerConfiguration configuration) noexcept
    : decoder_(decoder),
      queue_(queue),
      format_(format),
      generations_(generations),
      decode_generation_(decode_generation),
      configuration_(configuration),
      next_buffer_id_(configuration.first_buffer_id) {}

bool DecodeRenderQueueProducer::Impl::allocate() noexcept {
    for (auto& block : blocks_) {
        block.bytes.reset(new (std::nothrow) std::byte[
            configuration_.maximum_decode_block_bytes]);
        if (!block.bytes) {
            return false;
        }
    }
    return true;
}

std::unique_ptr<DecodeRenderQueueProducer>
DecodeRenderQueueProducer::create(
    decode::AudioDecodeBoundary& decoder,
    render::SpscAudioBlockQueue& queue,
    contract::RenderFormat format,
    render::RenderGenerationSet generations,
    decode::DecodeGeneration decode_generation,
    DecodeRenderQueueProducerConfiguration configuration) noexcept {
    const auto queue_snapshot = queue.snapshot();
    const auto bytes_per_frame = format.format.bytes_per_frame();
    if (!format.is_valid()
        || !configuration.is_valid()
        || !queue_snapshot.is_valid()
        || bytes_per_frame == 0
        || configuration.maximum_chunk_frames
            > std::numeric_limits<size_t>::max() / bytes_per_frame
        || static_cast<size_t>(configuration.maximum_chunk_frames)
                * bytes_per_frame
            > queue_snapshot.maximum_bytes_per_slot
        || configuration.maximum_decode_block_bytes < bytes_per_frame) {
        return nullptr;
    }
    auto impl = std::unique_ptr<Impl>{new (std::nothrow) Impl(
        decoder,
        queue,
        format,
        generations,
        decode_generation,
        configuration)};
    if (!impl || !impl->allocate()) {
        return nullptr;
    }
    return std::unique_ptr<DecodeRenderQueueProducer>{
        new (std::nothrow) DecodeRenderQueueProducer(std::move(impl))};
}

DecodeRenderQueueProducer::DecodeRenderQueueProducer(
    std::unique_ptr<Impl> impl) noexcept
    : impl_(std::move(impl)) {}

DecodeRenderQueueProducer::~DecodeRenderQueueProducer() = default;

DecodeRenderQueueProducerResult DecodeRenderQueueProducer::step() noexcept {
    return impl_
        ? impl_->step()
        : DecodeRenderQueueProducerResult{};
}

DecodeRenderQueueProducerResult
DecodeRenderQueueProducer::prefetch() noexcept {
    return impl_
        ? impl_->prefetch()
        : DecodeRenderQueueProducerResult{};
}

DecodeRenderQueueProducerSnapshot
DecodeRenderQueueProducer::snapshot() const noexcept {
    return impl_
        ? impl_->snapshot()
        : DecodeRenderQueueProducerSnapshot{};
}

} // namespace kivo::core::playback
