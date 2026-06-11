#include <array>
#include <cstddef>
#include <iostream>
#include <memory>
#include <stdexcept>

#include "kivo/core/playback/pipeline/decode_render_queue_producer.hpp"

namespace {

using namespace kivo;

#define PIPELINE_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            throw std::runtime_error("Assertion failed: " #condition); \
        } \
    } while (false)

[[nodiscard]] constexpr core::contract::RenderFormat format() noexcept {
    using namespace core::contract;
    return {{
        SampleFormat::Float32,
        ChannelLayout::Stereo,
        FrameLayout::Interleaved,
        {},
        48000,
        32,
        32
    }};
}

[[nodiscard]] constexpr core::contract::GenerationId generation(
    uint64_t value) noexcept {
    auto result = core::contract::GenerationId::initial();
    for (uint64_t index = 0; index < value; ++index) {
        result = core::contract::GenerationId::next(result);
    }
    return result;
}

class ScriptedDecoder final : public core::decode::AudioDecodeBoundary {
public:
    std::array<core::decode::DecodedAudioBlockView, 2> blocks{};
    size_t block_count{0};
    size_t next_block{0};
    core::decode::DecodeFailure terminal_failure{
        core::decode::DecodeFailure::None};

    [[nodiscard]] core::decode::DecodeOpenResult open(
        std::unique_ptr<core::decode::ByteSourceBoundary>,
        const core::decode::DecodeOpenRequest&) noexcept override {
        return core::decode::DecodeOpenResult::failed(
            core::decode::DecodeFailure::BoundaryFailure);
    }

    [[nodiscard]] core::decode::DecodeStepResult decode_next() noexcept override {
        if (next_block < block_count) {
            return core::decode::DecodeStepResult::produced(
                blocks[next_block++]);
        }
        return terminal_failure == core::decode::DecodeFailure::None
            ? core::decode::DecodeStepResult::end_of_stream()
            : core::decode::DecodeStepResult::failed(terminal_failure);
    }

    [[nodiscard]] core::decode::DecodeControlResult seek(
        core::contract::SamplePosition,
        core::decode::DecodeGeneration) noexcept override {
        return core::decode::DecodeControlResult::no_op();
    }

    [[nodiscard]] core::decode::DecodeControlResult flush(
        core::decode::DecodeGeneration) noexcept override {
        return core::decode::DecodeControlResult::no_op();
    }

    [[nodiscard]] core::decode::DecodeControlResult close() noexcept override {
        return core::decode::DecodeControlResult::succeeded();
    }

    [[nodiscard]] core::decode::MediaProbe probe() const noexcept override {
        return {};
    }
};

template <size_t Size>
[[nodiscard]] core::decode::DecodedAudioBlockView decoded_block(
    const std::array<std::byte, Size>& bytes,
    core::contract::FrameCount frames,
    core::contract::SamplePosition offset,
    uint64_t source_generation,
    uint64_t decode_generation) noexcept {
    return {
        bytes,
        format(),
        frames,
        offset,
        {generation(source_generation)},
        {generation(decode_generation)},
        false
    };
}

[[nodiscard]] std::unique_ptr<
    core::playback::DecodeRenderQueueProducer> producer(
    ScriptedDecoder& decoder,
    core::render::SpscAudioBlockQueue& queue,
    size_t maximum_bytes,
    core::contract::FrameCount chunk_frames) {
    core::render::RenderGenerationSet generations{};
    generations.seek.id = generation(5);
    generations.flush.id = generation(6);
    generations.device.id = generation(7);
    return core::playback::DecodeRenderQueueProducer::create(
        decoder,
        queue,
        format(),
        generations,
        {maximum_bytes, chunk_frames, {1}});
}

void lookahead_marks_only_final_block_eos() {
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
    PIPELINE_ASSERT(front->generations.stream.id == generation(2));
    PIPELINE_ASSERT(front->buffer_generation.id == generation(3));
    PIPELINE_ASSERT(queue->consume_front(4));
    front = queue->try_peek();
    PIPELINE_ASSERT(front);
    PIPELINE_ASSERT(front->end_of_stream);
    PIPELINE_ASSERT(front->frame_offset == 4);
    PIPELINE_ASSERT(queue->producer_closed());
}

void chunking_preserves_offsets_and_terminal_tail() {
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

void full_queue_reports_backpressure_without_losing_pending() {
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

void oversized_decode_block_fails_before_queue_mutation() {
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
    ScriptedDecoder decoder;
    auto queue = core::render::SpscAudioBlockQueue::create({2, 16});
    PIPELINE_ASSERT(queue);
    PIPELINE_ASSERT(!producer(decoder, *queue, 64, 3));
}

} // namespace

int main() {
    struct Test {
        const char* name;
        void (*run)();
    };
    const Test tests[] = {
        {"lookahead_marks_only_final_block_eos",
         lookahead_marks_only_final_block_eos},
        {"chunking_preserves_offsets_and_terminal_tail",
         chunking_preserves_offsets_and_terminal_tail},
        {"full_queue_reports_backpressure_without_losing_pending",
         full_queue_reports_backpressure_without_losing_pending},
        {"oversized_decode_block_fails_before_queue_mutation",
         oversized_decode_block_fails_before_queue_mutation},
        {"creation_rejects_chunk_larger_than_queue_slot",
         creation_rejects_chunk_larger_than_queue_slot}
    };
    int passed = 0;
    std::cout << "=== Playback Pipeline Tests ===\n\n";
    for (const auto& test : tests) {
        std::cout << "  " << test.name << "... ";
        try {
            test.run();
            ++passed;
            std::cout << "PASS\n";
        } catch (const std::exception& exception) {
            std::cout << "FAIL: " << exception.what() << "\n";
        }
    }
    std::cout << "\n=== " << passed << "/" << std::size(tests)
              << " passed ===\n";
    return passed == std::size(tests) ? 0 : 1;
}
