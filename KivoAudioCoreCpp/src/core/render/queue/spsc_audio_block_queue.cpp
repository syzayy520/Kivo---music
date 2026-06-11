#include "kivo/core/render/queue/spsc_audio_block_queue.hpp"

#include <array>
#include <atomic>
#include <cstring>
#include <limits>
#include <new>

namespace kivo::core::render {

namespace {

template <typename Value>
void saturating_increment(std::atomic<Value>& value, Value amount = 1) noexcept {
    auto current = value.load(std::memory_order_relaxed);
    while (true) {
        const auto maximum = std::numeric_limits<Value>::max();
        const auto next = amount > maximum - current ? maximum : current + amount;
        if (value.compare_exchange_weak(
                current,
                next,
                std::memory_order_relaxed,
                std::memory_order_relaxed)) {
            return;
        }
    }
}

template <typename Value>
void saturating_decrement(std::atomic<Value>& value, Value amount) noexcept {
    auto current = value.load(std::memory_order_relaxed);
    while (true) {
        const auto next = amount > current ? Value{0} : current - amount;
        if (value.compare_exchange_weak(
                current,
                next,
                std::memory_order_relaxed,
                std::memory_order_relaxed)) {
            return;
        }
    }
}

} // namespace

struct SpscAudioBlockQueue::SlotMetadata {
    contract::RenderFormat format{};
    contract::FrameCount frame_count{0};
    contract::FrameCount consumed_frames{0};
    contract::SamplePosition frame_offset{contract::kInvalidSamplePosition};
    contract::BufferId buffer_id{};
    contract::BufferGeneration buffer_generation{};
    RenderGenerationSet generations{};
    bool end_of_stream{false};
};

namespace {

struct alignas(64) CacheLineAtomic {
    std::atomic<uint64_t> value{0};
    std::array<std::byte, 64 - sizeof(std::atomic<uint64_t>)> padding{};
};

static_assert(sizeof(CacheLineAtomic) == 64);

} // namespace

struct alignas(64) SpscAudioBlockQueue::AtomicState {
    CacheLineAtomic producer{};
    CacheLineAtomic consumer{};
    CacheLineAtomic closed{};
    CacheLineAtomic pushed_blocks{};
    CacheLineAtomic consumed_blocks{};
    CacheLineAtomic rejected_full{};
    CacheLineAtomic rejected_oversized{};
    CacheLineAtomic queued_frames{};
};

std::unique_ptr<SpscAudioBlockQueue> SpscAudioBlockQueue::create(
    SpscAudioBlockQueueConfiguration configuration) noexcept {
    if (!configuration.is_valid()) {
        return {};
    }
    auto queue = std::unique_ptr<SpscAudioBlockQueue>{
        new (std::nothrow) SpscAudioBlockQueue(configuration)};
    if (!queue || !queue->storage_ready()) {
        return {};
    }
    return queue;
}

SpscAudioBlockQueue::SpscAudioBlockQueue(
    SpscAudioBlockQueueConfiguration configuration) noexcept
    : configuration_(configuration),
      slots_(new (std::nothrow) SlotMetadata[configuration.slot_count]),
      bytes_(new (std::nothrow) std::byte[configuration.storage_bytes()]),
      state_(new (std::nothrow) AtomicState{}) {}

SpscAudioBlockQueue::~SpscAudioBlockQueue() = default;

SpscQueuePushResult SpscAudioBlockQueue::try_push(
    const AudioBlockView& block) noexcept {
    if (state_->closed.value.load(std::memory_order_acquire) != 0) {
        return SpscQueuePushResult::Closed;
    }
    if (!block.is_valid()) {
        return SpscQueuePushResult::InvalidBlock;
    }
    if (block.bytes.size() > configuration_.maximum_bytes_per_slot) {
        saturating_increment(state_->rejected_oversized.value);
        return SpscQueuePushResult::BlockTooLarge;
    }

    const auto producer = state_->producer.value.load(std::memory_order_relaxed);
    const auto consumer = state_->consumer.value.load(std::memory_order_acquire);
    if (producer - consumer >= configuration_.slot_count) {
        saturating_increment(state_->rejected_full.value);
        return SpscQueuePushResult::Full;
    }

    auto& metadata = slot(producer);
    std::memcpy(slot_bytes(producer), block.bytes.data(), block.bytes.size());
    metadata.format = block.format;
    metadata.frame_count = block.frame_count;
    metadata.consumed_frames = 0;
    metadata.frame_offset = block.frame_offset;
    metadata.buffer_id = block.buffer_id;
    metadata.buffer_generation = block.buffer_generation;
    metadata.generations = block.generations;
    metadata.end_of_stream = block.end_of_stream;

    saturating_increment(state_->pushed_blocks.value);
    saturating_increment(state_->queued_frames.value, block.frame_count);
    state_->producer.value.store(producer + 1, std::memory_order_release);
    return SpscQueuePushResult::Pushed;
}

std::optional<AudioBlockView> SpscAudioBlockQueue::try_peek() const noexcept {
    const auto consumer = state_->consumer.value.load(std::memory_order_relaxed);
    const auto producer = state_->producer.value.load(std::memory_order_acquire);
    if (consumer == producer) {
        return std::nullopt;
    }

    const auto& metadata = slot(consumer);
    const auto remaining_frames = metadata.frame_count - metadata.consumed_frames;
    const auto bytes_per_frame = metadata.format.format.bytes_per_frame();
    const auto consumed_bytes =
        static_cast<size_t>(metadata.consumed_frames) * bytes_per_frame;
    const auto remaining_bytes =
        static_cast<size_t>(remaining_frames) * bytes_per_frame;
    return AudioBlockView{
        std::span<const std::byte>{
            slot_bytes(consumer) + consumed_bytes,
            remaining_bytes},
        metadata.format,
        remaining_frames,
        metadata.frame_offset + metadata.consumed_frames,
        metadata.buffer_id,
        metadata.buffer_generation,
        metadata.generations,
        metadata.end_of_stream
    };
}

bool SpscAudioBlockQueue::consume_front(
    contract::FrameCount accepted_frames) noexcept {
    const auto consumer = state_->consumer.value.load(std::memory_order_relaxed);
    const auto producer = state_->producer.value.load(std::memory_order_acquire);
    if (consumer == producer || accepted_frames == 0) {
        return false;
    }

    auto& metadata = slot(consumer);
    const auto remaining_frames = metadata.frame_count - metadata.consumed_frames;
    if (accepted_frames > remaining_frames) {
        return false;
    }

    metadata.consumed_frames += accepted_frames;
    saturating_decrement(state_->queued_frames.value, accepted_frames);
    if (metadata.consumed_frames == metadata.frame_count) {
        saturating_increment(state_->consumed_blocks.value);
        state_->consumer.value.store(consumer + 1, std::memory_order_release);
    }
    return true;
}

void SpscAudioBlockQueue::close_producer() noexcept {
    state_->closed.value.store(1, std::memory_order_release);
}

bool SpscAudioBlockQueue::producer_closed() const noexcept {
    return state_->closed.value.load(std::memory_order_acquire) != 0;
}

bool SpscAudioBlockQueue::empty() const noexcept {
    return state_->consumer.value.load(std::memory_order_acquire)
        == state_->producer.value.load(std::memory_order_acquire);
}

SpscAudioBlockQueueSnapshot SpscAudioBlockQueue::snapshot() const noexcept {
    const auto consumer = state_->consumer.value.load(std::memory_order_acquire);
    const auto producer = state_->producer.value.load(std::memory_order_acquire);
    const auto used = producer - consumer;
    return {
        configuration_.slot_count,
        static_cast<uint32_t>(
            used > configuration_.slot_count ? configuration_.slot_count : used),
        configuration_.maximum_bytes_per_slot,
        state_->queued_frames.value.load(std::memory_order_relaxed),
        state_->pushed_blocks.value.load(std::memory_order_relaxed),
        state_->consumed_blocks.value.load(std::memory_order_relaxed),
        state_->rejected_full.value.load(std::memory_order_relaxed),
        state_->rejected_oversized.value.load(std::memory_order_relaxed),
        producer_closed()
    };
}

void SpscAudioBlockQueue::reset() noexcept {
    state_->producer.value.store(0, std::memory_order_relaxed);
    state_->consumer.value.store(0, std::memory_order_relaxed);
    state_->closed.value.store(0, std::memory_order_relaxed);
    state_->pushed_blocks.value.store(0, std::memory_order_relaxed);
    state_->consumed_blocks.value.store(0, std::memory_order_relaxed);
    state_->rejected_full.value.store(0, std::memory_order_relaxed);
    state_->rejected_oversized.value.store(0, std::memory_order_relaxed);
    state_->queued_frames.value.store(0, std::memory_order_relaxed);
}

bool SpscAudioBlockQueue::storage_ready() const noexcept {
    return slots_ != nullptr && bytes_ != nullptr && state_ != nullptr;
}

SpscAudioBlockQueue::SlotMetadata& SpscAudioBlockQueue::slot(
    uint64_t ordinal) noexcept {
    return slots_[ordinal % configuration_.slot_count];
}

const SpscAudioBlockQueue::SlotMetadata& SpscAudioBlockQueue::slot(
    uint64_t ordinal) const noexcept {
    return slots_[ordinal % configuration_.slot_count];
}

std::byte* SpscAudioBlockQueue::slot_bytes(uint64_t ordinal) noexcept {
    return bytes_.get()
        + (ordinal % configuration_.slot_count) * configuration_.maximum_bytes_per_slot;
}

const std::byte* SpscAudioBlockQueue::slot_bytes(uint64_t ordinal) const noexcept {
    return bytes_.get()
        + (ordinal % configuration_.slot_count) * configuration_.maximum_bytes_per_slot;
}

} // namespace kivo::core::render
