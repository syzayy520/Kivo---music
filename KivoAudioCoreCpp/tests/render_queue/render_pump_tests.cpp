#include "render_queue_tests_main.hpp"

#include <array>
#include <cstddef>

#include "kivo/core/render/pump/render_pump.hpp"

namespace {

using namespace std::chrono_literals;
using namespace kivo;

class ScriptedWriteBoundary final : public core::render::RenderWriteBoundary {
public:
    core::render::RenderWriteResult result =
        core::render::RenderWriteResult::Delayed(1, 1ms);
    core::contract::FrameCount observed_frames{0};
    core::contract::SamplePosition observed_offset{0};

    [[nodiscard]] core::render::RenderWriteResult write(
        const core::render::AudioBlockView& block) noexcept override {
        observed_frames = block.frame_count;
        observed_offset = block.frame_offset;
        return result;
    }
};

[[nodiscard]] constexpr core::contract::RenderFormat pump_format() noexcept {
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
[[nodiscard]] core::render::AudioBlockView pump_block(
    const std::array<std::byte, Size>& bytes,
    core::contract::FrameCount frames,
    bool end_of_stream = false) noexcept {
    return {
        bytes,
        pump_format(),
        frames,
        100,
        {1},
        {},
        {},
        end_of_stream
    };
}

void empty_and_drained_are_distinct() {
    auto queue = core::render::SpscAudioBlockQueue::create({2, 64});
    RENDER_QUEUE_ASSERT(queue);
    ScriptedWriteBoundary boundary;
    core::render::RenderPump pump(*queue, boundary);

    RENDER_QUEUE_ASSERT(
        pump.step().disposition == core::render::RenderPumpDisposition::Idle);
    queue->close_producer();
    RENDER_QUEUE_ASSERT(
        pump.step().disposition
        == core::render::RenderPumpDisposition::QueueDrained);
}

void full_write_consumes_front() {
    auto queue = core::render::SpscAudioBlockQueue::create({2, 64});
    RENDER_QUEUE_ASSERT(queue);
    const std::array<std::byte, 16> bytes{};
    RENDER_QUEUE_ASSERT(
        queue->try_push(pump_block(bytes, 4))
        == core::render::SpscQueuePushResult::Pushed);
    ScriptedWriteBoundary boundary;
    boundary.result = core::render::RenderWriteResult::Full(4);
    core::render::RenderPump pump(*queue, boundary);

    const auto result = pump.step();
    RENDER_QUEUE_ASSERT(
        result.disposition == core::render::RenderPumpDisposition::Progress);
    RENDER_QUEUE_ASSERT(result.accepted_frames == 4);
    RENDER_QUEUE_ASSERT(queue->empty());
}

void partial_write_retains_exact_tail_and_eos() {
    auto queue = core::render::SpscAudioBlockQueue::create({2, 64});
    RENDER_QUEUE_ASSERT(queue);
    const std::array<std::byte, 16> bytes{};
    RENDER_QUEUE_ASSERT(
        queue->try_push(pump_block(bytes, 4, true))
        == core::render::SpscQueuePushResult::Pushed);
    ScriptedWriteBoundary boundary;
    boundary.result = core::render::RenderWriteResult::Partial(4, 1);
    core::render::RenderPump pump(*queue, boundary);

    RENDER_QUEUE_ASSERT(
        pump.step().disposition == core::render::RenderPumpDisposition::Progress);
    const auto tail = queue->try_peek();
    RENDER_QUEUE_ASSERT(tail);
    RENDER_QUEUE_ASSERT(tail->frame_count == 3);
    RENDER_QUEUE_ASSERT(tail->frame_offset == 101);
    RENDER_QUEUE_ASSERT(tail->end_of_stream);

    boundary.result = core::render::RenderWriteResult::Full(3);
    const auto eos = pump.step();
    RENDER_QUEUE_ASSERT(
        eos.disposition == core::render::RenderPumpDisposition::EndOfStream);
    RENDER_QUEUE_ASSERT(queue->empty());
}

void delayed_write_preserves_front_and_retry() {
    auto queue = core::render::SpscAudioBlockQueue::create({2, 64});
    RENDER_QUEUE_ASSERT(queue);
    const std::array<std::byte, 16> bytes{};
    RENDER_QUEUE_ASSERT(
        queue->try_push(pump_block(bytes, 4))
        == core::render::SpscQueuePushResult::Pushed);
    ScriptedWriteBoundary boundary;
    boundary.result = core::render::RenderWriteResult::Delayed(4, 7ms);
    core::render::RenderPump pump(*queue, boundary);

    const auto result = pump.step();
    RENDER_QUEUE_ASSERT(
        result.disposition == core::render::RenderPumpDisposition::Delayed);
    RENDER_QUEUE_ASSERT(result.retry_after == 7ms);
    RENDER_QUEUE_ASSERT(queue->snapshot().used_slots == 1);
}

void stale_rejection_discards_only_stale_block() {
    auto queue = core::render::SpscAudioBlockQueue::create({2, 64});
    RENDER_QUEUE_ASSERT(queue);
    const std::array<std::byte, 16> bytes{};
    RENDER_QUEUE_ASSERT(
        queue->try_push(pump_block(bytes, 4))
        == core::render::SpscQueuePushResult::Pushed);
    ScriptedWriteBoundary boundary;
    boundary.result = core::render::RenderWriteResult::Rejected(
        4,
        core::render::RenderFailure::StaleSeekGeneration);
    core::render::RenderPump pump(*queue, boundary);

    const auto result = pump.step();
    RENDER_QUEUE_ASSERT(
        result.disposition
        == core::render::RenderPumpDisposition::DiscardedStale);
    RENDER_QUEUE_ASSERT(
        result.failure == core::render::RenderFailure::StaleSeekGeneration);
    RENDER_QUEUE_ASSERT(queue->empty());
}

void nonstale_rejection_and_failure_preserve_front() {
    auto queue = core::render::SpscAudioBlockQueue::create({2, 64});
    RENDER_QUEUE_ASSERT(queue);
    const std::array<std::byte, 16> bytes{};
    RENDER_QUEUE_ASSERT(
        queue->try_push(pump_block(bytes, 4))
        == core::render::SpscQueuePushResult::Pushed);
    ScriptedWriteBoundary boundary;
    core::render::RenderPump pump(*queue, boundary);

    boundary.result = core::render::RenderWriteResult::Rejected(
        4,
        core::render::RenderFailure::UnsupportedFormat);
    RENDER_QUEUE_ASSERT(
        pump.step().disposition == core::render::RenderPumpDisposition::Rejected);
    RENDER_QUEUE_ASSERT(queue->snapshot().used_slots == 1);

    boundary.result = core::render::RenderWriteResult::Failed(
        4,
        core::render::RenderFailure::DeviceLost);
    RENDER_QUEUE_ASSERT(
        pump.step().disposition == core::render::RenderPumpDisposition::Failed);
    RENDER_QUEUE_ASSERT(queue->snapshot().used_slots == 1);
}

} // namespace

void run_render_pump_tests(RenderQueueTestRunner& runner) {
    runner.run("empty_and_drained_are_distinct", empty_and_drained_are_distinct);
    runner.run("full_write_consumes_front", full_write_consumes_front);
    runner.run(
        "partial_write_retains_exact_tail_and_eos",
        partial_write_retains_exact_tail_and_eos);
    runner.run(
        "delayed_write_preserves_front_and_retry",
        delayed_write_preserves_front_and_retry);
    runner.run(
        "stale_rejection_discards_only_stale_block",
        stale_rejection_discards_only_stale_block);
    runner.run(
        "nonstale_rejection_and_failure_preserve_front",
        nonstale_rejection_and_failure_preserve_front);
}
