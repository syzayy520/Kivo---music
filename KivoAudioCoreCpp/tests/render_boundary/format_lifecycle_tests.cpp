#include "render_boundary_tests_main.hpp"

#include <chrono>

#include "kivo/core/render/format/render_open_request.hpp"
#include "kivo/core/render/format/render_open_result.hpp"
#include "kivo/core/render/lifecycle/render_control_request.hpp"
#include "kivo/core/render/lifecycle/render_control_result.hpp"

namespace {

using namespace kivo::core;

contract::RenderFormat stereo_float_format() {
    return {{
        contract::SampleFormat::Float32,
        contract::ChannelLayout::Stereo,
        contract::FrameLayout::Interleaved,
        {},
        48000,
        32,
        32
    }};
}

render::RendererCapabilitySnapshot capabilities_for_open() {
    render::RendererCapabilitySnapshot capabilities{};
    capabilities.identity = {11};
    capabilities.render_capability.render_level = contract::CapabilityLevel::Standard;
    capabilities.render_capability.max_sample_rate_ceiling = 96000;
    capabilities.render_capability.max_channels_ceiling = 2;
    capabilities.buffer_capacity_frames = 1024;
    capabilities.maximum_write_frames = 256;
    capabilities.feature_bits = render::render_feature_bit(render::RenderFeature::Drain);
    return capabilities;
}

void open_request_requires_format_and_capacity() {
    render::RenderOpenRequest request{};
    RENDER_ASSERT(!request.is_valid());

    request.requested_format = stereo_float_format();
    request.requested_buffer_frames = 1024;
    request.format_policy = render::FormatAcceptancePolicy::Exact;
    RENDER_ASSERT(request.is_valid());
}

void accepted_open_result_carries_negotiated_truth() {
    const auto format = stereo_float_format();
    const auto capabilities = capabilities_for_open();
    const auto result = render::RenderOpenResult::Accepted(format, 1024, capabilities);

    RENDER_ASSERT(result.is_accepted());
    RENDER_ASSERT(result.disposition() == render::RenderOpenDisposition::Accepted);
    RENDER_ASSERT(result.failure() == render::RenderFailure::None);
    RENDER_ASSERT(result.accepted_format() == format);
    RENDER_ASSERT(result.buffer_frames() == 1024);
    RENDER_ASSERT(result.capabilities() == capabilities);
}

void invalid_accepted_open_result_becomes_failure() {
    const auto result = render::RenderOpenResult::Accepted({}, 0, {});
    RENDER_ASSERT(!result.is_accepted());
    RENDER_ASSERT(result.disposition() == render::RenderOpenDisposition::Failed);
    RENDER_ASSERT(result.failure() == render::RenderFailure::BoundaryFailure);
}

void accepted_open_result_cannot_exceed_capability_capacity() {
    const auto result =
        render::RenderOpenResult::Accepted(stereo_float_format(), 2048, capabilities_for_open());
    RENDER_ASSERT(!result.is_accepted());
    RENDER_ASSERT(result.failure() == render::RenderFailure::BoundaryFailure);
}

void control_results_keep_failure_and_state_consistent() {
    const auto success =
        render::RenderControlResult::Succeeded(render::RenderLifecycleState::Started);
    RENDER_ASSERT(success.is_success());
    RENDER_ASSERT(success.failure() == render::RenderFailure::None);

    const auto timeout =
        render::RenderControlResult::TimedOut(render::RenderLifecycleState::Draining);
    RENDER_ASSERT(!timeout.is_success());
    RENDER_ASSERT(timeout.failure() == render::RenderFailure::Timeout);
    RENDER_ASSERT(timeout.state() == render::RenderLifecycleState::Draining);
}

void drain_request_rejects_negative_timeout() {
    RENDER_ASSERT(render::RenderDrainRequest{std::chrono::nanoseconds{0}}.is_valid());
    RENDER_ASSERT(!render::RenderDrainRequest{std::chrono::nanoseconds{-1}}.is_valid());
}

} // namespace

void run_render_format_lifecycle_tests(RenderBoundaryTestRunner& runner) {
    runner.run("open_request_requires_format_and_capacity", open_request_requires_format_and_capacity);
    runner.run(
        "accepted_open_result_carries_negotiated_truth",
        accepted_open_result_carries_negotiated_truth);
    runner.run(
        "invalid_accepted_open_result_becomes_failure",
        invalid_accepted_open_result_becomes_failure);
    runner.run(
        "accepted_open_result_cannot_exceed_capability_capacity",
        accepted_open_result_cannot_exceed_capability_capacity);
    runner.run(
        "control_results_keep_failure_and_state_consistent",
        control_results_keep_failure_and_state_consistent);
    runner.run("drain_request_rejects_negative_timeout", drain_request_rejects_negative_timeout);
}
