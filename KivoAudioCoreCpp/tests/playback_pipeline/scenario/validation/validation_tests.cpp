#include "../../fixture/pipeline_test_assert.hpp"
#include "../../fixture/pipeline_test_factory.hpp"
#include "../../runner/playback_pipeline_test_cases.hpp"

namespace playback_pipeline_test {

void oversized_decode_block_fails_before_queue_mutation() {
    using namespace kivo;
    const std::array<std::byte, 40> bytes{};
    ScriptedDecoder decoder;
    decoder.blocks[0] = decoded_block(bytes, 5, 0, 2, 3);
    decoder.block_count = 1;
    auto queue = core::render::SpscAudioBlockQueue::create({2, 40});
    PIPELINE_ASSERT(queue);
    auto pipeline = producer(decoder, *queue, 32, 5);
    PIPELINE_ASSERT(pipeline);

    const auto result = pipeline->step();
    PIPELINE_ASSERT(
        result.disposition
        == core::playback::DecodeRenderQueueProducerDisposition::Failed);
    PIPELINE_ASSERT(
        result.decode_failure == core::decode::DecodeFailure::InvalidBuffer);
    PIPELINE_ASSERT(queue->empty());
}

void creation_rejects_chunk_larger_than_queue_slot() {
    using namespace kivo;
    ScriptedDecoder decoder;
    auto queue = core::render::SpscAudioBlockQueue::create({2, 16});
    PIPELINE_ASSERT(queue);
    PIPELINE_ASSERT(!producer(decoder, *queue, 64, 3));
}

} // namespace playback_pipeline_test
