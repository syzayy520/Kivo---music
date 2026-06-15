#include "../../fixture/pipeline_test_assert.hpp"
#include "../../fixture/pipeline_test_factory.hpp"
#include "../../runner/playback_pipeline_test_cases.hpp"

namespace playback_pipeline_test {

void full_queue_reports_backpressure_without_losing_pending() {
    using namespace kivo;
    const std::array<std::byte, 8> bytes{};
    auto queue = core::render::SpscAudioBlockQueue::create({2, 8});
    PIPELINE_ASSERT(queue);
    const core::render::AudioBlockView filler{
        bytes, format(), 1, 0, {99}, {}, {}, false};
    PIPELINE_ASSERT(
        queue->try_push(filler)
        == core::render::SpscQueuePushResult::Pushed);
    PIPELINE_ASSERT(
        queue->try_push(filler)
        == core::render::SpscQueuePushResult::Pushed);

    ScriptedDecoder decoder;
    decoder.blocks[0] = decoded_block(bytes, 1, 0, 2, 3);
    decoder.block_count = 1;
    auto pipeline = producer(decoder, *queue, 8, 1);
    PIPELINE_ASSERT(pipeline);
    static_cast<void>(pipeline->step());
    const auto pressure = pipeline->step();
    PIPELINE_ASSERT(
        pressure.disposition
        == core::playback::DecodeRenderQueueProducerDisposition::Backpressure);
    PIPELINE_ASSERT(pressure.queued_frames == 0);
    PIPELINE_ASSERT(pipeline->snapshot().backpressure_count == 1);
    PIPELINE_ASSERT(!queue->producer_closed());
}

} // namespace playback_pipeline_test
