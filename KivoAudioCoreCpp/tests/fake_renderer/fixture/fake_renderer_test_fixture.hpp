#pragma once

#include <cstddef>
#include <span>

#include "kivo/core/contract/generation_id.hpp"
#include "kivo/testing/render/renderer/fake_renderer.hpp"

namespace fake_renderer_test {

inline kivo::core::contract::RenderFormat stereo_float_format() {
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

inline kivo::testing::render::FakeRendererConfiguration configuration(
    size_t observation_capacity = 64) {
    using namespace kivo::core;
    kivo::testing::render::FakeRendererConfiguration config{};
    config.supported_format = stereo_float_format();
    config.capabilities.identity = {101};
    config.capabilities.render_capability.render_level = contract::CapabilityLevel::High;
    config.capabilities.render_capability.path = contract::CapabilityPath::DedicatedPath;
    config.capabilities.render_capability.max_sample_rate_ceiling = 192000;
    config.capabilities.render_capability.max_channels_ceiling = 8;
    config.capabilities.buffer_capacity_frames = 1024;
    config.capabilities.maximum_write_frames = 256;
    config.capabilities.feature_bits =
        render::render_feature_bit(render::RenderFeature::PartialWrite)
        | render::render_feature_bit(render::RenderFeature::DelayReport)
        | render::render_feature_bit(render::RenderFeature::Flush)
        | render::render_feature_bit(render::RenderFeature::Drain)
        | render::render_feature_bit(render::RenderFeature::Reset);
    config.observation_capacity = observation_capacity;
    return config;
}

inline kivo::core::render::RenderOpenRequest open_request(
    const kivo::testing::render::FakeRendererConfiguration& config) {
    kivo::core::render::RenderOpenRequest request{};
    request.requested_format = config.supported_format;
    request.requested_buffer_frames = config.capabilities.buffer_capacity_frames;
    request.generations.device = config.capabilities.device_generation;
    return request;
}

inline kivo::core::render::AudioBlockView block(
    std::span<const std::byte> bytes,
    kivo::core::contract::FrameCount frames,
    kivo::core::contract::SamplePosition offset,
    kivo::core::render::RenderGenerationSet generations) {
    return {
        bytes,
        stereo_float_format(),
        frames,
        offset,
        {1},
        {},
        generations,
        false
    };
}

inline kivo::core::render::RenderGenerationSet next_stream(
    kivo::core::render::RenderGenerationSet generations) {
    generations.stream.id =
        kivo::core::contract::GenerationId::next(generations.stream.id);
    return generations;
}

inline kivo::core::render::RenderGenerationSet next_seek(
    kivo::core::render::RenderGenerationSet generations) {
    generations.seek.id =
        kivo::core::contract::GenerationId::next(generations.seek.id);
    return generations;
}

inline kivo::core::render::RenderGenerationSet next_flush(
    kivo::core::render::RenderGenerationSet generations) {
    generations.flush.id =
        kivo::core::contract::GenerationId::next(generations.flush.id);
    return generations;
}

inline kivo::core::render::RenderGenerationSet next_device(
    kivo::core::render::RenderGenerationSet generations) {
    generations.device.id =
        kivo::core::contract::GenerationId::next(generations.device.id);
    return generations;
}

} // namespace fake_renderer_test
