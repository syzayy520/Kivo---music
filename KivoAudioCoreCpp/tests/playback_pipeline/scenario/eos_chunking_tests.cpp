#include "../fixture/pipeline_test_assert.hpp"
#include "../fixture/pipeline_test_factory.hpp"
#include "../runner/playback_pipeline_test_cases.hpp"

namespace playback_pipeline_test {

void lookahead_marks_only_final_block_eos() {
    using namespace kivo;
    const std::array<std::byte, 32> first{};
    const std::array<std::byte, 32> second{};
    ScriptedDecoder decoder;
    decoder.blocks[0] = decoded_block(first, 4, 0, 2, 3);
    decoder.blocks[1] = decoded_block(second, 4, 4, 2, 3);
    decoder.block_count = 2;
    auto queue = core::render::SpscAudioBlockQueue::create({4, 32});
    PIPELINE_ASSERT(queue);
    auto pipeline = producer(decoder, *queue, 32, 4);
    PIPELINE_ASSERT(pipeline);

    PIPELINE_ASSERT(
        pipeline->step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::Primed);
    PIPELINE_ASSERT(
        pipeline->step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::Queued);
    PIPELINE_ASSERT(
        pipeline->step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::EndOfStream);

    auto front = queue->try_peek();
    PIPELINE_ASSERT(front);
    PIPELINE_ASSERT(!front->end_of_stream);
    PIPELINE_ASSERT(queue->consume_front(4));
    front = queue->try_peek();
    PIPELINE_ASSERT(front);
    PIPELINE_ASSERT(front->end_of_stream);
    PIPELINE_ASSERT(front->frame_offset == 4);
    PIPELINE_ASSERT(queue->producer_closed());
}

void chunking_preserves_offsets_and_terminal_tail() {
    using namespace kivo;
    const std::array<std::byte, 64> bytes{};
    ScriptedDecoder decoder;
    decoder.blocks[0] = decoded_block(bytes, 8, 100, 2, 3);
    decoder.block_count = 1;
    auto queue = core::render::SpscAudioBlockQueue::create({4, 24});
    PIPELINE_ASSERT(queue);
    auto pipeline = producer(decoder, *queue, 64, 3);
    PIPELINE_ASSERT(pipeline);

    PIPELINE_ASSERT(
        pipeline->step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::Primed);
    PIPELINE_ASSERT(pipeline->step().queued_frames == 3);
    PIPELINE_ASSERT(pipeline->step().queued_frames == 3);
    const auto final = pipeline->step();
    PIPELINE_ASSERT(
        final.disposition
        == core::playback::DecodeRenderQueueProducerDisposition::EndOfStream);
    PIPELINE_ASSERT(final.queued_frames == 2);

    for (const auto expected_offset : {100u, 103u, 106u}) {
        const auto front = queue->try_peek();
        PIPELINE_ASSERT(front);
        PIPELINE_ASSERT(front->frame_offset == expected_offset);
        PIPELINE_ASSERT(queue->consume_front(front->frame_count));
    }
    const auto snapshot = pipeline->snapshot();
    PIPELINE_ASSERT(snapshot.decoded_frames == 8);
    PIPELINE_ASSERT(snapshot.queued_frames == 8);
    PIPELINE_ASSERT(snapshot.queued_blocks == 3);
}

void non_final_track_keeps_queue_open_and_maps_timeline() {
    using namespace kivo;
    const std::array<std::byte, 32> bytes{};
    ScriptedDecoder decoder;
    decoder.blocks[0] = decoded_block(bytes, 4, 0, 2, 3);
    decoder.block_count = 1;
    auto queue = core::render::SpscAudioBlockQueue::create({2, 32});
    PIPELINE_ASSERT(queue);
    auto pipeline = producer(
        decoder,
        *queue,
        32,
        4,
        400,
        core::playback::QueueEndOfStreamPolicy::KeepOpen);
    PIPELINE_ASSERT(pipeline);

    PIPELINE_ASSERT(
        pipeline->step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::Primed);
    PIPELINE_ASSERT(
        pipeline->step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::EndOfStream);
    const auto front = queue->try_peek();
    PIPELINE_ASSERT(front);
    PIPELINE_ASSERT(front->frame_offset == 400);
    PIPELINE_ASSERT(!front->end_of_stream);
    PIPELINE_ASSERT(!queue->producer_closed());
}

void timeline_offset_overflow_fails_before_queue_mutation() {
    using namespace kivo;
    const std::array<std::byte, 32> bytes{};
    ScriptedDecoder decoder;
    decoder.blocks[0] = decoded_block(bytes, 4, 1, 2, 3);
    decoder.block_count = 1;
    auto queue = core::render::SpscAudioBlockQueue::create({2, 32});
    PIPELINE_ASSERT(queue);
    auto pipeline = producer(
        decoder,
        *queue,
        32,
        4,
        core::contract::kInvalidSamplePosition - 2);
    PIPELINE_ASSERT(pipeline);

    const auto result = pipeline->step();
    PIPELINE_ASSERT(
        result.disposition
        == core::playback::DecodeRenderQueueProducerDisposition::Failed);
    PIPELINE_ASSERT(
        result.decode_failure == core::decode::DecodeFailure::InvalidBuffer);
    PIPELINE_ASSERT(queue->empty());
}

} // namespace playback_pipeline_test
