#include "kivo/testing/render/renderer/fake_renderer.hpp"

#include <algorithm>

namespace kivo::testing::render {

using core::render::RenderFailure;
using core::render::RenderLifecycleState;

FakeRenderer::FakeRenderer(
    FakeRendererConfiguration configuration,
    time::ManualClock& clock) noexcept
    : configuration_(configuration),
      clock_(clock),
      observations_(configuration.observation_capacity) {}

core::render::RendererIdentity FakeRenderer::identity() const noexcept {
    return configuration_.capabilities.identity;
}

core::render::RendererCapabilitySnapshot FakeRenderer::capabilities() const noexcept {
    return configuration_.capabilities;
}

core::render::RenderOpenResult FakeRenderer::open(
    const core::render::RenderOpenRequest& request) noexcept {
    if (snapshot_.state != RenderLifecycleState::Closed) {
        record(RenderObservationOperation::Open, RenderFailure::InvalidState);
        return core::render::RenderOpenResult::Rejected(RenderFailure::InvalidState);
    }
    if (!configuration_.is_valid()) {
        snapshot_.state = RenderLifecycleState::Failed;
        snapshot_.last_failure = RenderFailure::BoundaryFailure;
        record(RenderObservationOperation::Open, RenderFailure::BoundaryFailure);
        return core::render::RenderOpenResult::Failed(RenderFailure::BoundaryFailure);
    }
    if (!request.is_valid()) {
        record(RenderObservationOperation::Open, RenderFailure::InvalidFormat);
        return core::render::RenderOpenResult::Rejected(RenderFailure::InvalidFormat);
    }
    if (request.generations.device != configuration_.capabilities.device_generation) {
        record(RenderObservationOperation::Open, RenderFailure::StaleDeviceGeneration);
        return core::render::RenderOpenResult::Rejected(
            RenderFailure::StaleDeviceGeneration);
    }
    if (request.output_mode == core::render::RenderOutputMode::Exclusive
        && !configuration_.capabilities.supports(
            core::render::RenderFeature::ExclusiveMode)) {
        record(RenderObservationOperation::Open, RenderFailure::UnsupportedFormat);
        return core::render::RenderOpenResult::Rejected(
            RenderFailure::UnsupportedFormat);
    }

    auto accepted_format = request.requested_format;
    if (request.requested_format != configuration_.supported_format) {
        if (request.format_policy == core::render::FormatAcceptancePolicy::Exact) {
            record(RenderObservationOperation::Open, RenderFailure::UnsupportedFormat);
            return core::render::RenderOpenResult::Rejected(
                RenderFailure::UnsupportedFormat);
        }
        accepted_format = configuration_.supported_format;
    }

    const auto buffer_frames = std::min(
        request.requested_buffer_frames,
        configuration_.capabilities.buffer_capacity_frames);
    snapshot_.state = RenderLifecycleState::Open;
    snapshot_.accepted_format = accepted_format;
    snapshot_.buffer_capacity_frames = buffer_frames;
    snapshot_.buffered_frames = 0;
    snapshot_.total_accepted_frames = 0;
    snapshot_.generations = request.generations;
    snapshot_.last_failure = RenderFailure::None;
    write_count_ = 0;
    consume_count_ = 0;

    record(RenderObservationOperation::Open);
    return core::render::RenderOpenResult::Accepted(
        accepted_format,
        buffer_frames,
        configuration_.capabilities);
}

} // namespace kivo::testing::render
