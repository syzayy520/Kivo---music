#include "render_queue_tests_main.hpp"

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <thread>

#include "kivo/core/render/queue/spsc_audio_block_queue.hpp"

namespace {

using namespace kivo;

[[nodiscard]] constexpr core::contract::RenderFormat test_format() noexcept {
    using namespace core::contract;
    return {{
        SampleFormat::Int16,
        ChannelLayout::Stereo,
        FrameLayout::Interleaved,
        {},
        48000,
        16,
        16
    }};
}

template <size_t Size>
[[nodiscard]] core::render::AudioBlockView block_view(
    const std::array<std::byte, Size>& bytes,
    core::contract::FrameCount frames,
    core::contract::SamplePosition offset,
    uint64_t buffer_id,
    bool end_of_stream = false) noexcept {
    return {
        bytes,
        test_format(),
        frames,
        offset,
        {buffer_id},
        {core::contract::GenerationId::next(
            core::contract::GenerationId::initial())},
        {},
        end_of_stream
    };
}

void invalid_configuration_is_rejected() {
    using core::render::SpscAudioBlockQueue;
    RENDER_QUEUE_ASSERT(!SpscAudioBlockQueue::create({0, 1024}));
    RENDER_QUEUE_ASSERT(!SpscAudioBlockQueue::create({1, 1024}));
    RENDER_QUEUE_ASSERT(!SpscAudioBlockQueue::create({2, 0}));
    RENDER_QUEUE_ASSERT(!SpscAudioBlockQueue::create({4097, 1024}));
    RENDER_QUEUE_ASSERT(!SpscAudioBlockQueue::create({4096, 1024 * 1024}));
}

void fifo_metadata_and_bytes_are_preserved() {
    auto queue = core::render::SpscAudioBlockQueue::create({4, 64});
    RENDER_QUEUE_ASSERT(queue);
    const std::array<std::byte, 8> first{
        std::byte{1}, std::byte{2}, std::byte{3}, std::byte{4},
        std::byte{5}, std::byte{6}, std::byte{7}, std::byte{8}};
    const std::array<std::byte, 8> second{
        std::byte{11}, std::byte{12}, std::byte{13}, std::byte{14},
        std::byte{15}, std::byte{16}, std::byte{17}, std::byte{18}};

    RENDER_QUEUE_ASSERT(
        queue->try_push(block_view(first, 2, 100, 1))
        == core::render::SpscQueuePushResult::Pushed);
    RENDER_QUEUE_ASSERT(
        queue->try_push(block_view(second, 2, 102, 2, true))
        == core::render::SpscQueuePushResult::Pushed);

    const auto front = queue->try_peek();
    RENDER_QUEUE_ASSERT(front);
    RENDER_QUEUE_ASSERT(front->frame_offset == 100);
    RENDER_QUEUE_ASSERT(front->buffer_id.value == 1);
    RENDER_QUEUE_ASSERT(front->buffer_generation.value() == 1);
    RENDER_QUEUE_ASSERT(front->generations.stream.value() == 0);
    RENDER_QUEUE_ASSERT(!front->end_of_stream);
    RENDER_QUEUE_ASSERT(std::memcmp(front->bytes.data(), first.data(), first.size()) == 0);
    RENDER_QUEUE_ASSERT(queue->consume_front(2));

    const auto next = queue->try_peek();
    RENDER_QUEUE_ASSERT(next);
    RENDER_QUEUE_ASSERT(next->buffer_id.value == 2);
    RENDER_QUEUE_ASSERT(next->end_of_stream);
    RENDER_QUEUE_ASSERT(std::memcmp(next->bytes.data(), second.data(), second.size()) == 0);
}

void full_and_oversized_rejections_are_explicit() {
    auto queue = core::render::SpscAudioBlockQueue::create({2, 8});
    RENDER_QUEUE_ASSERT(queue);
    const std::array<std::byte, 8> bytes{};
    const std::array<std::byte, 12> oversized{};
    RENDER_QUEUE_ASSERT(
        queue->try_push(block_view(bytes, 2, 0, 1))
        == core::render::SpscQueuePushResult::Pushed);
    RENDER_QUEUE_ASSERT(
        queue->try_push(block_view(bytes, 2, 2, 2))
        == core::render::SpscQueuePushResult::Pushed);
    RENDER_QUEUE_ASSERT(
        queue->try_push(block_view(bytes, 2, 4, 3))
        == core::render::SpscQueuePushResult::Full);
    RENDER_QUEUE_ASSERT(queue->consume_front(2));
    RENDER_QUEUE_ASSERT(
        queue->try_push(block_view(oversized, 3, 4, 3))
        == core::render::SpscQueuePushResult::BlockTooLarge);

    const auto snapshot = queue->snapshot();
    RENDER_QUEUE_ASSERT(snapshot.rejected_full == 1);
    RENDER_QUEUE_ASSERT(snapshot.rejected_oversized == 1);
    RENDER_QUEUE_ASSERT(snapshot.observed_peak_used_slots == 2);
    queue->reset();
    RENDER_QUEUE_ASSERT(queue->snapshot().observed_peak_used_slots == 0);
}

void partial_consume_exposes_exact_tail() {
    auto queue = core::render::SpscAudioBlockQueue::create({2, 32});
    RENDER_QUEUE_ASSERT(queue);
    const std::array<std::byte, 16> bytes{
        std::byte{0}, std::byte{1}, std::byte{2}, std::byte{3},
        std::byte{4}, std::byte{5}, std::byte{6}, std::byte{7},
        std::byte{8}, std::byte{9}, std::byte{10}, std::byte{11},
        std::byte{12}, std::byte{13}, std::byte{14}, std::byte{15}};
    RENDER_QUEUE_ASSERT(
        queue->try_push(block_view(bytes, 4, 50, 9, true))
        == core::render::SpscQueuePushResult::Pushed);
    RENDER_QUEUE_ASSERT(queue->consume_front(1));

    const auto tail = queue->try_peek();
    RENDER_QUEUE_ASSERT(tail);
    RENDER_QUEUE_ASSERT(tail->frame_count == 3);
    RENDER_QUEUE_ASSERT(tail->frame_offset == 51);
    RENDER_QUEUE_ASSERT(tail->bytes.size() == 12);
    RENDER_QUEUE_ASSERT(tail->bytes.front() == std::byte{4});
    RENDER_QUEUE_ASSERT(tail->end_of_stream);
    RENDER_QUEUE_ASSERT(!queue->consume_front(4));
    RENDER_QUEUE_ASSERT(queue->consume_front(3));
    RENDER_QUEUE_ASSERT(queue->empty());
}

void close_allows_drain_but_rejects_new_pushes() {
    auto queue = core::render::SpscAudioBlockQueue::create({2, 8});
    RENDER_QUEUE_ASSERT(queue);
    const std::array<std::byte, 8> bytes{};
    RENDER_QUEUE_ASSERT(
        queue->try_push(block_view(bytes, 2, 0, 1))
        == core::render::SpscQueuePushResult::Pushed);
    queue->close_producer();
    RENDER_QUEUE_ASSERT(
        queue->try_push(block_view(bytes, 2, 2, 2))
        == core::render::SpscQueuePushResult::Closed);
    RENDER_QUEUE_ASSERT(queue->try_peek());
    RENDER_QUEUE_ASSERT(queue->consume_front(2));
    RENDER_QUEUE_ASSERT(queue->empty());
    RENDER_QUEUE_ASSERT(queue->producer_closed());
}

void wraparound_preserves_fifo_order() {
    auto queue = core::render::SpscAudioBlockQueue::create({3, 8});
    RENDER_QUEUE_ASSERT(queue);
    for (uint64_t index = 0; index < 30; ++index) {
        std::array<std::byte, 8> bytes{};
        bytes[0] = static_cast<std::byte>(index);
        RENDER_QUEUE_ASSERT(
            queue->try_push(block_view(bytes, 2, index * 2, index + 1))
            == core::render::SpscQueuePushResult::Pushed);
        const auto front = queue->try_peek();
        RENDER_QUEUE_ASSERT(front);
        RENDER_QUEUE_ASSERT(front->bytes.front() == static_cast<std::byte>(index));
        RENDER_QUEUE_ASSERT(queue->consume_front(2));
    }
    const auto snapshot = queue->snapshot();
    RENDER_QUEUE_ASSERT(snapshot.pushed_blocks == 30);
    RENDER_QUEUE_ASSERT(snapshot.consumed_blocks == 30);
    RENDER_QUEUE_ASSERT(snapshot.used_slots == 0);
    RENDER_QUEUE_ASSERT(snapshot.queued_frames == 0);
    RENDER_QUEUE_ASSERT(snapshot.observed_peak_used_slots == 1);
}

void two_thread_stress_preserves_order_without_sleep() {
    constexpr uint64_t block_count = 100000;
    auto queue = core::render::SpscAudioBlockQueue::create({64, 8});
    RENDER_QUEUE_ASSERT(queue);
    std::atomic<bool> failed{false};

    std::thread producer([&] {
        for (uint64_t index = 0; index < block_count; ++index) {
            std::array<std::byte, 8> bytes{};
            std::memcpy(bytes.data(), &index, sizeof(index));
            const auto block = block_view(bytes, 2, index * 2, index + 1);
            while (queue->try_push(block) == core::render::SpscQueuePushResult::Full) {
                std::this_thread::yield();
            }
        }
        queue->close_producer();
    });

    std::thread consumer([&] {
        uint64_t expected = 0;
        while (expected < block_count) {
            if (!queue->snapshot().is_valid()) {
                failed.store(true, std::memory_order_relaxed);
                return;
            }
            const auto front = queue->try_peek();
            if (!front) {
                std::this_thread::yield();
                continue;
            }
            uint64_t value = 0;
            std::memcpy(&value, front->bytes.data(), sizeof(value));
            if (value != expected || front->buffer_id.value != expected + 1) {
                failed.store(true, std::memory_order_relaxed);
                return;
            }
            if (!queue->consume_front(2)) {
                failed.store(true, std::memory_order_relaxed);
                return;
            }
            ++expected;
        }
    });

    producer.join();
    consumer.join();
    RENDER_QUEUE_ASSERT(!failed.load(std::memory_order_relaxed));
    const auto snapshot = queue->snapshot();
    RENDER_QUEUE_ASSERT(snapshot.pushed_blocks == block_count);
    RENDER_QUEUE_ASSERT(snapshot.consumed_blocks == block_count);
    RENDER_QUEUE_ASSERT(snapshot.used_slots == 0);
    RENDER_QUEUE_ASSERT(snapshot.observed_peak_used_slots >= 1);
    RENDER_QUEUE_ASSERT(snapshot.observed_peak_used_slots <= 64);
}

} // namespace

void run_spsc_audio_block_queue_tests(RenderQueueTestRunner& runner) {
    runner.run("invalid_configuration_is_rejected", invalid_configuration_is_rejected);
    runner.run(
        "fifo_metadata_and_bytes_are_preserved",
        fifo_metadata_and_bytes_are_preserved);
    runner.run(
        "full_and_oversized_rejections_are_explicit",
        full_and_oversized_rejections_are_explicit);
    runner.run(
        "partial_consume_exposes_exact_tail",
        partial_consume_exposes_exact_tail);
    runner.run(
        "close_allows_drain_but_rejects_new_pushes",
        close_allows_drain_but_rejects_new_pushes);
    runner.run("wraparound_preserves_fifo_order", wraparound_preserves_fifo_order);
    runner.run(
        "two_thread_stress_preserves_order_without_sleep",
        two_thread_stress_preserves_order_without_sleep);
}
