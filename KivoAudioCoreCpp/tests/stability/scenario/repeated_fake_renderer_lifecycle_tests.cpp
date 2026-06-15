#include "../fixture/stability_test_runner.hpp"
#include "../runner/stability_test_cases.hpp"

#include <array>
#include <cstddef>

#include "kivo/testing/render/renderer/fake_renderer.hpp"

namespace {

[[nodiscard]] kivo::testing::render::FakeRendererConfiguration
configuration() {
    using namespace kivo;
    using namespace core::contract;

    testing::render::FakeRendererConfiguration result{};
    result.supported_format = {{
        SampleFormat::Float32,
        ChannelLayout::Stereo,
        FrameLayout::Interleaved,
        {},
        48000,
        32,
        32
    }};
    result.capabilities.identity = {7001};
    result.capabilities.render_capability.render_level = CapabilityLevel::High;
    result.capabilities.render_capability.path = CapabilityPath::DedicatedPath;
    result.capabilities.render_capability.max_sample_rate_ceiling = 192000;
    result.capabilities.render_capability.max_channels_ceiling = 8;
    result.capabilities.buffer_capacity_frames = 256;
    result.capabilities.maximum_write_frames = 128;
    result.observation_capacity = 16;
    return result;
}

} // namespace

void repeated_fake_renderer_lifecycle_is_bounded(
    const StabilityTestContext&) {
    using namespace kivo;

    constexpr uint32_t cycle_count = 2000;
    constexpr core::contract::FrameCount frames = 128;
    std::array<std::byte, frames * 8> bytes{};

    testing::time::ManualClock clock;
    const auto config = configuration();
    testing::render::FakeRenderer renderer{config, clock};

    for (uint32_t cycle = 0; cycle < cycle_count; ++cycle) {
        core::render::RenderOpenRequest request{};
        request.requested_format = config.supported_format;
        request.requested_buffer_frames =
            config.capabilities.buffer_capacity_frames;
        request.generations.device =
            config.capabilities.device_generation;

        STABILITY_ASSERT(renderer.open(request).is_accepted());
        STABILITY_ASSERT(renderer.start().is_success());

        const core::render::AudioBlockView block{
            bytes,
            config.supported_format,
            frames,
            cycle * frames,
            {1},
            {},
            request.generations,
            false
        };
        STABILITY_ASSERT(renderer.write(block).accepted_frames() == frames);
        STABILITY_ASSERT(renderer.consume(frames).consumed_frames == frames);
        STABILITY_ASSERT(renderer.stop().is_success());
        STABILITY_ASSERT(renderer.close().is_success());
        STABILITY_ASSERT(
            renderer.snapshot().state
            == core::render::RenderLifecycleState::Closed);
        STABILITY_ASSERT(renderer.write_count() == 0);
        STABILITY_ASSERT(renderer.consume_count() == 0);
        STABILITY_ASSERT(renderer.observation_count() <= 16);
    }
}
