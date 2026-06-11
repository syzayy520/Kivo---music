#include "kivo/core/playback/pipeline/decode_render_queue_producer.hpp"

#include <algorithm>
#include <array>
#include <cstring>
#include <limits>
#include <new>
#include <optional>

namespace kivo::core::playback {

namespace {

struct BufferedBlock {
    std::unique_ptr<std::byte[]> bytes{};
    size_t byte_count{0};
    contract::FrameCount frame_count{0};
    contract::SamplePosition frame_offset{0};
    contract::SourceGeneration source_generation{};
    decode::DecodeGeneration decode_generation{};
};

} // namespace

class DecodeRenderQueueProducer::Impl {
public:
    Impl(
        decode::AudioDecodeBoundary& decoder,
        render::SpscAudioBlockQueue& queue,
        contract::RenderFormat format,
        render::RenderGenerationSet generations,
        DecodeRenderQueueProducerConfiguration configuration) noexcept
        : decoder_(decoder),
          queue_(queue),
          format_(format),
          generations_(generations),
          configuration_(configuration),
          next_buffer_id_(configuration.first_buffer_id) {}

    [[nodiscard]] bool allocate() noexcept {
        for (auto& block : blocks_) {
            block.bytes.reset(new (std::nothrow) std::byte[
                configuration_.maximum_decode_block_bytes]);
            if (!block.bytes) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] DecodeRenderQueueProducerResult step() noexcept {
        if (snapshot_.state == DecodeRenderQueueProducerState::Failed) {
            return failed(snapshot_.last_decode_failure);
        }
        if (snapshot_.state == DecodeRenderQueueProducerState::EndOfStream) {
            return {
                DecodeRenderQueueProducerDisposition::EndOfStream,
                decode::DecodeFailure::None,
                render::SpscQueuePushResult::Closed,
                0
            };
        }
        snapshot_.state = DecodeRenderQueueProducerState::Running;

        if (active_index_) {
            return enqueue_active();
        }
        if (held_index_ && held_is_terminal_) {
            activate_held(true);
            return enqueue_active();
        }

        const auto decoded = decoder_.decode_next();
        switch (decoded.disposition()) {
        case decode::DecodeStepDisposition::Produced:
            if (!store_decoded(decoded.block())) {
                return failed(snapshot_.last_decode_failure);
            }
            if (!held_index_) {
                held_index_ = stored_index_;
                held_is_terminal_ = decoded.block().end_of_stream;
                return {
                    DecodeRenderQueueProducerDisposition::Primed,
                    decode::DecodeFailure::None,
                    render::SpscQueuePushResult::Pushed,
                    0
                };
            }
            active_index_ = held_index_;
            active_cursor_ = 0;
            active_is_terminal_ = false;
            held_index_ = stored_index_;
            held_is_terminal_ = decoded.block().end_of_stream;
            return enqueue_active();

        case decode::DecodeStepDisposition::EndOfStream:
            if (held_index_) {
                activate_held(true);
                return enqueue_active();
            }
            queue_.close_producer();
            snapshot_.state = DecodeRenderQueueProducerState::EndOfStream;
            return {
                DecodeRenderQueueProducerDisposition::EndOfStream,
                decode::DecodeFailure::None,
                render::SpscQueuePushResult::Closed,
                0
            };

        case decode::DecodeStepDisposition::Failed:
            return failed(decoded.failure());
        }
        return failed(decode::DecodeFailure::BoundaryFailure);
    }

    [[nodiscard]] DecodeRenderQueueProducerSnapshot snapshot() const noexcept {
        return snapshot_;
    }

private:
    [[nodiscard]] bool store_decoded(
        const decode::DecodedAudioBlockView& block) noexcept {
        if (!block.is_valid()
            || block.format != format_
            || block.bytes.size()
                > configuration_.maximum_decode_block_bytes) {
            snapshot_.last_decode_failure =
                block.bytes.size() > configuration_.maximum_decode_block_bytes
                ? decode::DecodeFailure::InvalidBuffer
                : decode::DecodeFailure::BoundaryFailure;
            snapshot_.state = DecodeRenderQueueProducerState::Failed;
            return false;
        }
        const size_t index = held_index_ ? 1u - *held_index_ : 0u;
        auto& stored = blocks_[index];
        std::memcpy(stored.bytes.get(), block.bytes.data(), block.bytes.size());
        stored.byte_count = block.bytes.size();
        stored.frame_count = block.frame_count;
        stored.frame_offset = block.frame_offset;
        stored.source_generation = block.source_generation;
        stored.decode_generation = block.decode_generation;
        stored_index_ = index;
        saturating_add(snapshot_.decoded_frames, block.frame_count);
        saturating_increment(snapshot_.decoded_blocks);
        return true;
    }

    void activate_held(bool terminal) noexcept {
        active_index_ = held_index_;
        active_cursor_ = 0;
        active_is_terminal_ = terminal;
        held_index_.reset();
        held_is_terminal_ = false;
    }

    [[nodiscard]] DecodeRenderQueueProducerResult enqueue_active() noexcept {
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
        const auto byte_count = static_cast<size_t>(frames) * bytes_per_frame;
        auto generations = generations_;
        generations.stream.id = block.source_generation.id;
        const render::AudioBlockView view{
            std::span<const std::byte>{block.bytes.get(), block.byte_count}
                .subspan(byte_offset, byte_count),
            format_,
            frames,
            block.frame_offset + active_cursor_,
            next_buffer_id_,
            {block.decode_generation.id},
            generations,
            active_is_terminal_ && frames == remaining
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
                queue_.close_producer();
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

    [[nodiscard]] DecodeRenderQueueProducerResult failed(
        decode::DecodeFailure failure) noexcept {
        snapshot_.state = DecodeRenderQueueProducerState::Failed;
        snapshot_.last_decode_failure =
            failure == decode::DecodeFailure::None
            ? decode::DecodeFailure::BoundaryFailure
            : failure;
        return {
            DecodeRenderQueueProducerDisposition::Failed,
            snapshot_.last_decode_failure,
            render::SpscQueuePushResult::Closed,
            0
        };
    }

    static void saturating_increment(uint64_t& value) noexcept {
        if (value != std::numeric_limits<uint64_t>::max()) {
            ++value;
        }
    }

    static void saturating_add(
        contract::FrameCount& value,
        contract::FrameCount amount) noexcept {
        const auto maximum =
            std::numeric_limits<contract::FrameCount>::max();
        value = amount > maximum - value ? maximum : value + amount;
    }

    decode::AudioDecodeBoundary& decoder_;
    render::SpscAudioBlockQueue& queue_;
    contract::RenderFormat format_{};
    render::RenderGenerationSet generations_{};
    DecodeRenderQueueProducerConfiguration configuration_{};
    std::array<BufferedBlock, 2> blocks_{};
    std::optional<size_t> held_index_{};
    std::optional<size_t> active_index_{};
    size_t stored_index_{0};
    contract::FrameCount active_cursor_{0};
    bool held_is_terminal_{false};
    bool active_is_terminal_{false};
    contract::BufferId next_buffer_id_{};
    DecodeRenderQueueProducerSnapshot snapshot_{};
};

std::unique_ptr<DecodeRenderQueueProducer>
DecodeRenderQueueProducer::create(
    decode::AudioDecodeBoundary& decoder,
    render::SpscAudioBlockQueue& queue,
    contract::RenderFormat format,
    render::RenderGenerationSet generations,
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

DecodeRenderQueueProducerSnapshot
DecodeRenderQueueProducer::snapshot() const noexcept {
    return impl_
        ? impl_->snapshot()
        : DecodeRenderQueueProducerSnapshot{};
}

} // namespace kivo::core::playback
