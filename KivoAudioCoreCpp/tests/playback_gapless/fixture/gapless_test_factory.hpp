#pragma once

#include <array>
#include <memory>

#include "kivo/core/playback/gapless/plan/gapless_track_plan.hpp"
#include "kivo/core/playback/gapless/coordinator/gapless_track_prepare_request.hpp"
#include "kivo/core/playback/pipeline/producer/decode_render_queue_producer.hpp"

#include "gapless_test_decoder.hpp"

namespace playback_gapless_test {

[[nodiscard]] constexpr kivo::core::contract::RenderFormat
format(uint32_t sample_rate = 48000) noexcept {
    using namespace kivo::core::contract;
    return {{
        SampleFormat::Float32,
        ChannelLayout::Stereo,
        FrameLayout::Interleaved,
        {},
        sample_rate,
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
    kivo::core::contract::FrameCount frames) noexcept {
    return {
        bytes,
        format(),
        frames,
        0,
        {generation(2)},
        {generation(3)},
        false
    };
}

[[nodiscard]] inline std::unique_ptr<
    kivo::core::playback::DecodeRenderQueueProducer> producer(
    ScriptedDecoder& decoder,
    kivo::core::render::SpscAudioBlockQueue& queue,
    kivo::core::contract::SamplePosition origin,
    kivo::core::playback::QueueEndOfStreamPolicy policy,
    uint64_t first_buffer_id) {
    kivo::core::render::RenderGenerationSet generations{};
    generations.stream.id = generation(2);
    return kivo::core::playback::DecodeRenderQueueProducer::create(
        decoder,
        queue,
        format(),
        generations,
        {generation(3)},
        {32, 4, {first_buffer_id}, origin, policy});
}

[[nodiscard]] constexpr kivo::core::playback::GaplessTrackPlan plan(
    uint32_t index,
    kivo::core::contract::FrameCount frames,
    kivo::core::contract::SamplePosition origin,
    uint32_t sample_rate = 48000,
    bool final_track = false) noexcept {
    return {
        index,
        frames,
        origin,
        format(sample_rate),
        {},
        final_track
    };
}

[[nodiscard]] inline kivo::core::playback::GaplessTrackPrepareRequest
prepare_request(
    ScriptedDecoder& decoder,
    kivo::core::playback::GaplessTrackPlan track_plan,
    uint64_t first_buffer_id) {
    kivo::core::render::RenderGenerationSet generations{};
    generations.stream.id = generation(2);
    return {
        &decoder,
        track_plan,
        generations,
        {generation(3)},
        {32, 4, {first_buffer_id}, 0, {}}
    };
}

} // namespace playback_gapless_test
