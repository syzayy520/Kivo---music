#include "../fixture/pipeline_test_assert.hpp"
#include "../fixture/pipeline_test_factory.hpp"
#include "../runner/playback_pipeline_test_cases.hpp"

namespace playback_pipeline_test {

void stale_decode_generation_fails_before_queue_mutation() {
    using namespace kivo;
    const std::array<std::byte, 8> bytes{};
    ScriptedDecoder decoder;
    decoder.blocks[0] = decoded_block(bytes, 1, 0, 2, 2);
    decoder.block_count = 1;
    auto queue = core::render::SpscAudioBlockQueue::create({2, 8});
    PIPELINE_ASSERT(queue);
    auto pipeline = producer(decoder, *queue, 8, 1);
    PIPELINE_ASSERT(pipeline);

    const auto result = pipeline->step();
    PIPELINE_ASSERT(
        result.disposition
        == core::playback::DecodeRenderQueueProducerDisposition::DiscardedStale);
    PIPELINE_ASSERT(
        result.decode_failure
        == core::decode::DecodeFailure::StaleDecodeGeneration);
    PIPELINE_ASSERT(queue->empty());
    PIPELINE_ASSERT(pipeline->snapshot().stale_blocks_discarded == 1);
}

void stale_source_generation_fails_before_queue_mutation() {
    using namespace kivo;
    const std::array<std::byte, 8> bytes{};
    ScriptedDecoder decoder;
    decoder.blocks[0] = decoded_block(bytes, 1, 0, 1, 3);
    decoder.block_count = 1;
    auto queue = core::render::SpscAudioBlockQueue::create({2, 8});
    PIPELINE_ASSERT(queue);
    auto pipeline = producer(decoder, *queue, 8, 1);
    PIPELINE_ASSERT(pipeline);

    const auto result = pipeline->step();
    PIPELINE_ASSERT(
        result.disposition
        == core::playback::DecodeRenderQueueProducerDisposition::DiscardedStale);
    PIPELINE_ASSERT(
        result.decode_failure
        == core::decode::DecodeFailure::StaleSourceGeneration);
    PIPELINE_ASSERT(queue->empty());
    PIPELINE_ASSERT(pipeline->snapshot().stale_blocks_discarded == 1);
}

} // namespace playback_pipeline_test
