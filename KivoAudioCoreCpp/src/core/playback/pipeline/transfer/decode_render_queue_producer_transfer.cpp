#include "../state/decode_render_queue_producer_state.hpp"

#include <algorithm>
#include <cstring>
#include <limits>
#include <span>

namespace kivo::core::playback {

pipeline_buffer::StoreResult
DecodeRenderQueueProducer::Impl::store_decoded(
    const decode::DecodedAudioBlockView& block) noexcept {
    if (!block.is_valid() || block.format != format_) {
        snapshot_.last_decode_failure =
            decode::DecodeFailure::BoundaryFailure;
        snapshot_.state = DecodeRenderQueueProducerState::Failed;
        return pipeline_buffer::StoreResult::Failed;
    }
    if (block.source_generation.id != generations_.stream.id) {
        snapshot_.last_decode_failure =
            decode::DecodeFailure::StaleSourceGeneration;
        saturating_increment(snapshot_.stale_blocks_discarded);
        return pipeline_buffer::StoreResult::DiscardedStale;
    }
    if (block.decode_generation != decode_generation_) {
        snapshot_.last_decode_failure =
            decode::DecodeFailure::StaleDecodeGeneration;
        saturating_increment(snapshot_.stale_blocks_discarded);
        return pipeline_buffer::StoreResult::DiscardedStale;
    }
    if (block.bytes.size()
        > configuration_.maximum_decode_block_bytes) {
        snapshot_.last_decode_failure =
            decode::DecodeFailure::InvalidBuffer;
        snapshot_.state = DecodeRenderQueueProducerState::Failed;
        return pipeline_buffer::StoreResult::Failed;
    }
    const auto maximum =
        std::numeric_limits<contract::SamplePosition>::max();
    if (block.frame_offset
            > maximum - configuration_.timeline_origin_frames
        || block.frame_count
            > maximum
                - configuration_.timeline_origin_frames
                - block.frame_offset) {
        snapshot_.last_decode_failure =
            decode::DecodeFailure::InvalidBuffer;
        snapshot_.state = DecodeRenderQueueProducerState::Failed;
        return pipeline_buffer::StoreResult::Failed;
    }
    const size_t index = held_index_ ? 1u - *held_index_ : 0u;
    auto& stored = blocks_[index];
    std::memcpy(
        stored.bytes.get(),
        block.bytes.data(),
        block.bytes.size());
    const auto processed = processing_->process(
        std::span<std::byte>{stored.bytes.get(), block.bytes.size()},
        block.frame_count);
    if (!processed.succeeded()) {
        snapshot_.last_decode_failure =
            decode::DecodeFailure::ProcessingFailed;
        snapshot_.state = DecodeRenderQueueProducerState::Failed;
        return pipeline_buffer::StoreResult::Failed;
    }
    stored.byte_count = block.bytes.size();
    stored.frame_count = block.frame_count;
    stored.frame_offset = block.frame_offset;
    stored.source_generation = block.source_generation;
    stored.decode_generation = block.decode_generation;
    stored_index_ = index;
    saturating_add(snapshot_.decoded_frames, block.frame_count);
    saturating_increment(snapshot_.decoded_blocks);
    snapshot_.last_decode_failure = decode::DecodeFailure::None;
    return pipeline_buffer::StoreResult::Stored;
}

void DecodeRenderQueueProducer::Impl::activate_held(
    bool terminal) noexcept {
    active_index_ = held_index_;
    active_cursor_ = 0;
    active_is_terminal_ = terminal;
    held_index_.reset();
    held_is_terminal_ = false;
}

DecodeRenderQueueProducerResult
DecodeRenderQueueProducer::Impl::enqueue_active() noexcept {
    if (!active_index_) {
        return failed(decode::DecodeFailure::BoundaryFailure);
    }
    const auto& block = blocks_[*active_index_];
    const auto remaining = block.frame_count - active_cursor_;
    const auto frames = std::min(
        remaining,
        configuration_.maximum_chunk_frames);
    const auto bytes_per_frame = format_.format.bytes_per_frame();
    const auto byte_offset =
        static_cast<size_t>(active_cursor_) * bytes_per_frame;
    const auto byte_count =
        static_cast<size_t>(frames) * bytes_per_frame;
    auto generations = generations_;
    generations.stream.id = block.source_generation.id;
    const render::AudioBlockView view{
        std::span<const std::byte>{
            block.bytes.get(),
            block.byte_count}.subspan(byte_offset, byte_count),
        format_,
        frames,
        configuration_.timeline_origin_frames
            + block.frame_offset
            + active_cursor_,
        next_buffer_id_,
        {block.decode_generation.id},
        generations,
        configuration_.end_of_stream_policy
                == QueueEndOfStreamPolicy::CloseAndMarkFinal
            && active_is_terminal_
            && frames == remaining
    };
    const auto push = queue_.try_push(view);
    if (push == render::SpscQueuePushResult::Full) {
        saturating_increment(snapshot_.backpressure_count);
        return {
            DecodeRenderQueueProducerDisposition::Backpressure,
            decode::DecodeFailure::None,
            push,
            0
        };
    }
    if (push != render::SpscQueuePushResult::Pushed) {
        snapshot_.state = DecodeRenderQueueProducerState::Failed;
        snapshot_.last_decode_failure =
            decode::DecodeFailure::BoundaryFailure;
        return {
            DecodeRenderQueueProducerDisposition::Failed,
            snapshot_.last_decode_failure,
            push,
            0
        };
    }

    active_cursor_ += frames;
    saturating_add(snapshot_.queued_frames, frames);
    saturating_increment(snapshot_.queued_blocks);
    if (next_buffer_id_.value != std::numeric_limits<uint64_t>::max()) {
        ++next_buffer_id_.value;
    }
    if (active_cursor_ == block.frame_count) {
        const bool terminal = active_is_terminal_;
        active_index_.reset();
        active_cursor_ = 0;
        active_is_terminal_ = false;
        if (terminal) {
            if (configuration_.end_of_stream_policy
                == QueueEndOfStreamPolicy::CloseAndMarkFinal) {
                queue_.close_producer();
            }
            snapshot_.state =
                DecodeRenderQueueProducerState::EndOfStream;
            return {
                DecodeRenderQueueProducerDisposition::EndOfStream,
                decode::DecodeFailure::None,
                push,
                frames
            };
        }
    }
    return {
        DecodeRenderQueueProducerDisposition::Queued,
        decode::DecodeFailure::None,
        push,
        frames
    };
}

void DecodeRenderQueueProducer::Impl::saturating_increment(
    uint64_t& value) noexcept {
    if (value != std::numeric_limits<uint64_t>::max()) {
        ++value;
    }
}

void DecodeRenderQueueProducer::Impl::saturating_add(
    contract::FrameCount& value,
    contract::FrameCount amount) noexcept {
    const auto maximum =
        std::numeric_limits<contract::FrameCount>::max();
    value = amount > maximum - value ? maximum : value + amount;
}

} // namespace kivo::core::playback
