#include "queue_timing_measurement.hpp"

#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <thread>

#include "kivo/core/render/queue/spsc_audio_block_queue.hpp"

namespace kivo::validation::performance {

namespace {

using Clock = std::chrono::steady_clock;

[[nodiscard]] constexpr core::contract::RenderFormat queue_format() noexcept {
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

[[nodiscard]] constexpr core::contract::GenerationId generation_one() noexcept {
    return core::contract::GenerationId::next(
        core::contract::GenerationId::initial());
}

template <size_t Size>
[[nodiscard]] core::render::AudioBlockView block_view(
    const std::array<std::byte, Size>& bytes,
    uint64_t index) noexcept {
    constexpr core::contract::FrameCount frames = 48;
    return {
        bytes,
        queue_format(),
        frames,
        index * frames,
        {index + 1},
        {generation_one()},
        {},
        index + 1 == 256
    };
}

[[nodiscard]] uint64_t interval_ns(
    Clock::time_point before,
    Clock::time_point after) noexcept {
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            after - before).count());
}

} // namespace

QueueTimingEvidence measure_queue_timing() {
    constexpr uint64_t block_count = 256;
    auto queue = core::render::SpscAudioBlockQueue::create({64, 192});
    if (!queue) {
        throw std::runtime_error{"performance queue creation failed"};
    }

    QueueTimingEvidence result{};
    result.capacity_slots = 64;
    result.occupancy_samples.reserve(block_count);
    result.consumer_interval_ns.reserve(block_count - 1);
    std::atomic<bool> failed{false};

    std::thread producer([&] {
        for (uint64_t index = 0; index < block_count; ++index) {
            std::array<std::byte, 192> bytes{};
            std::memcpy(bytes.data(), &index, sizeof(index));
            const auto block = block_view(bytes, index);
            while (queue->try_push(block)
                == core::render::SpscQueuePushResult::Full) {
                std::this_thread::yield();
            }
            std::this_thread::sleep_for(std::chrono::microseconds{500});
        }
        queue->close_producer();
    });

    std::thread consumer([&] {
        Clock::time_point previous{};
        bool have_previous = false;
        for (uint64_t expected = 0; expected < block_count; ++expected) {
            auto front = queue->try_peek();
            while (!front) {
                std::this_thread::yield();
                front = queue->try_peek();
            }
            uint64_t value = 0;
            std::memcpy(&value, front->bytes.data(), sizeof(value));
            if (value != expected) {
                failed.store(true, std::memory_order_relaxed);
                return;
            }

            const auto now = Clock::now();
            if (have_previous) {
                result.consumer_interval_ns.push_back(
                    interval_ns(previous, now));
            }
            previous = now;
            have_previous = true;
            result.occupancy_samples.push_back(queue->snapshot().used_slots);
            if (!queue->consume_front(front->frame_count)) {
                failed.store(true, std::memory_order_relaxed);
                return;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds{1});
        }
    });

    producer.join();
    consumer.join();
    if (failed.load(std::memory_order_relaxed)) {
        throw std::runtime_error{"performance queue ordering failed"};
    }

    const auto snapshot = queue->snapshot();
    result.observed_peak_used_slots = snapshot.observed_peak_used_slots;
    result.successful_pushes = snapshot.pushed_blocks;
    result.successful_consumes = snapshot.consumed_blocks;
    if (result.occupancy_samples.size() != block_count
        || result.consumer_interval_ns.size() != block_count - 1) {
        throw std::runtime_error{"performance queue sample count mismatch"};
    }
    return result;
}

} // namespace kivo::validation::performance
