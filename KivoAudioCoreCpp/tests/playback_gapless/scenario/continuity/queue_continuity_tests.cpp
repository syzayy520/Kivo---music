#include "../../fixture/gapless_test_assert.hpp"
#include "../../fixture/gapless_test_factory.hpp"
#include "../../runner/playback_gapless_test_cases.hpp"

namespace playback_gapless_test {

void consecutive_tracks_preserve_pcm_and_eos() {
    using namespace kivo;
    std::array<std::byte, 32> first{};
    std::array<std::byte, 32> second{};
    first.fill(std::byte{0x11});
    second.fill(std::byte{0x22});
    ScriptedDecoder first_decoder;
    ScriptedDecoder second_decoder;
    first_decoder.blocks[0] = decoded_block(first, 4);
    first_decoder.block_count = 1;
    second_decoder.blocks[0] = decoded_block(second, 4);
    second_decoder.block_count = 1;
    auto queue = core::render::SpscAudioBlockQueue::create({4, 32});
    GAPLESS_ASSERT(queue);
    auto first_producer = producer(
        first_decoder,
        *queue,
        0,
        core::playback::QueueEndOfStreamPolicy::KeepOpen,
        1);
    auto second_producer = producer(
        second_decoder,
        *queue,
        4,
        core::playback::QueueEndOfStreamPolicy::CloseAndMarkFinal,
        2);
    GAPLESS_ASSERT(first_producer && second_producer);

    static_cast<void>(first_producer->step());
    GAPLESS_ASSERT(
        first_producer->step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::EndOfStream);
    GAPLESS_ASSERT(!queue->producer_closed());
    static_cast<void>(second_producer->step());
    GAPLESS_ASSERT(
        second_producer->step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::EndOfStream);
    GAPLESS_ASSERT(queue->producer_closed());

    auto block = queue->try_peek();
    GAPLESS_ASSERT(block && block->frame_offset == 0);
    GAPLESS_ASSERT(!block->end_of_stream);
    GAPLESS_ASSERT(block->bytes.front() == std::byte{0x11});
    GAPLESS_ASSERT(queue->consume_front(4));
    block = queue->try_peek();
    GAPLESS_ASSERT(block && block->frame_offset == 4);
    GAPLESS_ASSERT(block->end_of_stream);
    GAPLESS_ASSERT(block->bytes.front() == std::byte{0x22});
    GAPLESS_ASSERT(queue->consume_front(4));
    GAPLESS_ASSERT(queue->empty());
}

} // namespace playback_gapless_test
