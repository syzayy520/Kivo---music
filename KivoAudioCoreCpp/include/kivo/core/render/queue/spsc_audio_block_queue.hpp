#pragma once

#include <cstdint>
#include <memory>
#include <optional>

#include "kivo/core/render/buffer/audio_block_view.hpp"
#include "kivo/core/render/queue/spsc_audio_block_queue_configuration.hpp"
#include "kivo/core/render/queue/spsc_audio_block_queue_snapshot.hpp"

namespace kivo::core::render {

enum class SpscQueuePushResult : uint8_t {
    Pushed = 0,
    Full,
    InvalidBlock,
    BlockTooLarge,
    Closed
};

class SpscAudioBlockQueue final {
public:
    [[nodiscard]] static std::unique_ptr<SpscAudioBlockQueue> create(
        SpscAudioBlockQueueConfiguration configuration) noexcept;

    ~SpscAudioBlockQueue();

    SpscAudioBlockQueue(const SpscAudioBlockQueue&) = delete;
    SpscAudioBlockQueue& operator=(const SpscAudioBlockQueue&) = delete;
    SpscAudioBlockQueue(SpscAudioBlockQueue&&) = delete;
    SpscAudioBlockQueue& operator=(SpscAudioBlockQueue&&) = delete;

    [[nodiscard]] SpscQueuePushResult try_push(
        const AudioBlockView& block) noexcept;
    [[nodiscard]] std::optional<AudioBlockView> try_peek() const noexcept;
    [[nodiscard]] bool consume_front(
        contract::FrameCount accepted_frames) noexcept;

    void close_producer() noexcept;
    [[nodiscard]] bool producer_closed() const noexcept;
    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] SpscAudioBlockQueueSnapshot snapshot() const noexcept;

    // Requires producer and consumer to be quiescent.
    void reset() noexcept;

private:
    struct SlotMetadata;
    struct AtomicState;

    explicit SpscAudioBlockQueue(
        SpscAudioBlockQueueConfiguration configuration) noexcept;

    [[nodiscard]] bool storage_ready() const noexcept;
    [[nodiscard]] SlotMetadata& slot(uint64_t ordinal) noexcept;
    [[nodiscard]] const SlotMetadata& slot(uint64_t ordinal) const noexcept;
    [[nodiscard]] std::byte* slot_bytes(uint64_t ordinal) noexcept;
    [[nodiscard]] const std::byte* slot_bytes(uint64_t ordinal) const noexcept;

    SpscAudioBlockQueueConfiguration configuration_{};
    std::unique_ptr<SlotMetadata[]> slots_{};
    std::unique_ptr<std::byte[]> bytes_{};
    std::unique_ptr<AtomicState> state_{};
};

} // namespace kivo::core::render
