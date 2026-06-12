#pragma once

#include <array>
#include <memory>

#include "kivo/core/playback/pipeline/producer/decode_render_queue_producer.hpp"

#include "pipeline_test_decoder.hpp"

namespace playback_pipeline_test {

[[nodiscard]] constexpr kivo::core::contract::RenderFormat
format() noexcept {
    using namespace kivo::core::contract;
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

[[nodiscard]] constexpr kivo::core::contract::GenerationId generation(
    uint64_t value) noexcept {
    auto result = kivo::core::contract::GenerationId::initial();
    for (uint64_t index = 0; index < value; ++index) {
        result = kivo::core::contract::GenerationId::next(result);
    }
    return result;
}

template <size_t Size>
[[nodiscard]] kivo::core::decode::DecodedAudioBlockView decoded_block(
    const std::array<std::byte, Size>& bytes,
    kivo::core::contract::FrameCount frames,
    kivo::core::contract::SamplePosition offset,
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

[[nodiscard]] inline std::unique_ptr<
    kivo::core::playback::DecodeRenderQueueProducer> producer(
    ScriptedDecoder& decoder,
    kivo::core::render::SpscAudioBlockQueue& queue,
    size_t maximum_bytes,
    kivo::core::contract::FrameCount chunk_frames,
    kivo::core::contract::SamplePosition timeline_origin = 0,
    kivo::core::playback::QueueEndOfStreamPolicy end_policy =
        kivo::core::playback::QueueEndOfStreamPolicy::
            CloseAndMarkFinal,
    kivo::core::contract::BufferId first_buffer_id = {1}) {
    kivo::core::render::RenderGenerationSet generations{};
    generations.stream.id = generation(2);
    generations.seek.id = generation(5);
    generations.flush.id = generation(6);
    generations.device.id = generation(7);
    return kivo::core::playback::DecodeRenderQueueProducer::create(
        decoder,
        queue,
        format(),
        generations,
        {generation(3)},
        {
            maximum_bytes,
            chunk_frames,
            first_buffer_id,
            timeline_origin,
            end_policy
        });
}

} // namespace playback_pipeline_test
