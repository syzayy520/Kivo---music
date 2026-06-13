#include "../../fixture/pipeline_test_assert.hpp"
#include "../../fixture/pipeline_test_factory.hpp"
#include "../../runner/playback_pipeline_test_cases.hpp"

#include <array>
#include <cmath>
#include <cstring>

namespace playback_pipeline_test {

void processing_runs_before_queue_publication() {
    using namespace kivo;

    const std::array<float, 2> samples{0.8F, -0.8F};
    std::array<std::byte, sizeof(samples)> bytes{};
    std::memcpy(bytes.data(), samples.data(), bytes.size());

    ScriptedDecoder decoder;
    decoder.blocks[0] = decoded_block(bytes, 1, 0, 2, 3);
    decoder.block_count = 1;

    auto queue = core::render::SpscAudioBlockQueue::create(
        {2, bytes.size()});
    PIPELINE_ASSERT(queue != nullptr);

    core::processing::AudioProcessingConfiguration processing{};
    processing.volume.enabled = true;
    processing.volume.linear_gain = 0.5;

    auto pipeline = producer(
        decoder,
        *queue,
        bytes.size(),
        1,
        0,
        core::playback::QueueEndOfStreamPolicy::CloseAndMarkFinal,
        {1},
        processing);
    PIPELINE_ASSERT(pipeline != nullptr);

    PIPELINE_ASSERT(
        pipeline->step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::Primed);
    PIPELINE_ASSERT(
        pipeline->step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::EndOfStream);

    const auto queued = queue->try_peek();
    PIPELINE_ASSERT(queued.has_value());

    std::array<float, 2> output{};
    std::memcpy(
        output.data(),
        queued->bytes.data(),
        queued->bytes.size());
    PIPELINE_ASSERT(std::abs(output[0] - 0.4F) < 0.000001F);
    PIPELINE_ASSERT(std::abs(output[1] + 0.4F) < 0.000001F);

    const auto snapshot = pipeline->snapshot();
    PIPELINE_ASSERT(snapshot.processing.participation.volume_active);
    PIPELINE_ASSERT(snapshot.processing.processed_frames == 1);
    PIPELINE_ASSERT(snapshot.processing.processed_blocks == 1);
}

} // namespace playback_pipeline_test
