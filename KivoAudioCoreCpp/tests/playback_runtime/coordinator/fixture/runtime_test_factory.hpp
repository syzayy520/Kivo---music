#pragma once

#include <memory>

#include "kivo/core/playback/runtime/coordinator/playback_runtime_coordinator.hpp"
#include "kivo/testing/render/fake_renderer.hpp"

#include "runtime_test_byte_source.hpp"

namespace playback_runtime_test {

[[nodiscard]] constexpr kivo::core::contract::GenerationId generation(
    uint64_t value) noexcept {
    auto result = kivo::core::contract::GenerationId::initial();
    for (uint64_t index = 0; index < value; ++index) {
        result = kivo::core::contract::GenerationId::next(result);
    }
    return result;
}

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

[[nodiscard]] inline kivo::testing::render::FakeRendererConfiguration
renderer_configuration() noexcept {
    kivo::testing::render::FakeRendererConfiguration configuration{};
    configuration.supported_format = format();
    configuration.capabilities.identity = {71};
    configuration.capabilities.render_capability.render_level =
        kivo::core::contract::CapabilityLevel::High;
    configuration.capabilities.render_capability.path =
        kivo::core::contract::CapabilityPath::DedicatedPath;
    configuration.capabilities.render_capability.max_sample_rate_ceiling =
        192000;
    configuration.capabilities.render_capability.max_channels_ceiling = 8;
    configuration.capabilities.buffer_capacity_frames = 64;
    configuration.capabilities.maximum_write_frames = 16;
    configuration.capabilities.feature_bits =
        kivo::core::render::render_feature_bit(
            kivo::core::render::RenderFeature::PartialWrite)
        | kivo::core::render::render_feature_bit(
            kivo::core::render::RenderFeature::Flush)
        | kivo::core::render::render_feature_bit(
            kivo::core::render::RenderFeature::Drain)
        | kivo::core::render::render_feature_bit(
            kivo::core::render::RenderFeature::Reset);
    configuration.observation_capacity = 64;
    return configuration;
}

[[nodiscard]] constexpr kivo::core::playback::PlaybackCommand command(
    uint64_t id,
    kivo::core::contract::CommandKind kind,
    uint64_t session_generation,
    kivo::core::contract::SamplePosition position =
        kivo::core::contract::kInvalidSamplePosition) noexcept {
    return {
        {id},
        kind,
        {session_generation, kivo::core::contract::CommandOrigin::User},
        position
    };
}

[[nodiscard]] inline kivo::core::playback::PlaybackRuntimeOpenRequest
open_request(
    const kivo::testing::render::FakeRendererConfiguration& configuration,
    uint64_t decode_generation) noexcept {
    kivo::core::playback::PlaybackRuntimeOpenRequest request{};
    request.render_request.requested_format = format();
    request.render_request.requested_buffer_frames = 16;
    request.render_request.generations.device =
        configuration.capabilities.device_generation;
    request.queue_configuration = {8, 32};
    request.producer_configuration = {32, 4, {1}};
    request.decode_generation = {generation(decode_generation)};
    return request;
}

[[nodiscard]] inline std::unique_ptr<TestByteSource> source(
    uint64_t identity,
    uint64_t source_generation) {
    return std::make_unique<TestByteSource>(
        kivo::core::contract::SourceIdentity{identity},
        kivo::core::contract::SourceGeneration{
            generation(source_generation)});
}

} // namespace playback_runtime_test
