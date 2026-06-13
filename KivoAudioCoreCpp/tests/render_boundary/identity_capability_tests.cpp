#include "render_boundary_tests_main.hpp"

#include "kivo/core/render/capability/renderer_capability_snapshot.hpp"

namespace {

using namespace kivo::core;

render::RendererCapabilitySnapshot make_capabilities() {
    render::RendererCapabilitySnapshot capabilities{};
    capabilities.identity = {1};
    capabilities.render_capability.render_level = contract::CapabilityLevel::High;
    capabilities.render_capability.path = contract::CapabilityPath::DedicatedPath;
    capabilities.render_capability.max_sample_rate_ceiling = 192000;
    capabilities.render_capability.max_channels_ceiling = 8;
    capabilities.buffer_capacity_frames = 2048;
    capabilities.maximum_write_frames = 512;
    capabilities.feature_bits =
        render::render_feature_bit(render::RenderFeature::PartialWrite)
        | render::render_feature_bit(render::RenderFeature::Flush)
        | render::render_feature_bit(render::RenderFeature::Drain);
    return capabilities;
}

void renderer_identity_requires_nonzero_value() {
    RENDER_ASSERT(!render::RendererIdentity{}.is_valid());
    RENDER_ASSERT(render::RendererIdentity{7}.is_valid());
}

void capability_snapshot_requires_operational_limits() {
    auto capabilities = make_capabilities();
    RENDER_ASSERT(capabilities.is_valid());

    capabilities.maximum_write_frames = 4096;
    RENDER_ASSERT(!capabilities.is_valid());
}

void capability_features_are_explicit() {
    const auto capabilities = make_capabilities();
    RENDER_ASSERT(capabilities.supports(render::RenderFeature::PartialWrite));
    RENDER_ASSERT(capabilities.supports(render::RenderFeature::Drain));
    RENDER_ASSERT(!capabilities.supports(render::RenderFeature::Reset));
    RENDER_ASSERT(!capabilities.supports(render::RenderFeature::None));
}

} // namespace

void run_renderer_identity_capability_tests(RenderBoundaryTestRunner& runner) {
    runner.run("renderer_identity_requires_nonzero_value", renderer_identity_requires_nonzero_value);
    runner.run(
        "capability_snapshot_requires_operational_limits",
        capability_snapshot_requires_operational_limits);
    runner.run("capability_features_are_explicit", capability_features_are_explicit);
}
