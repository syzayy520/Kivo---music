#include "kivo/testing/render/renderer/fake_renderer.hpp"

namespace kivo::testing::render {

using core::render::RenderControlResult;
using core::render::RenderFailure;
using core::render::RenderLifecycleState;

RenderControlResult FakeRenderer::start() noexcept {
    if (snapshot_.state == RenderLifecycleState::Started) {
        record(RenderObservationOperation::Start);
        return RenderControlResult::NoOp(snapshot_.state);
    }
    if (snapshot_.state != RenderLifecycleState::Open
        && snapshot_.state != RenderLifecycleState::Stopped) {
        record(RenderObservationOperation::Start, RenderFailure::InvalidState);
        return RenderControlResult::Rejected(RenderFailure::InvalidState, snapshot_.state);
    }

    snapshot_.state = RenderLifecycleState::Started;
    record(RenderObservationOperation::Start);
    return RenderControlResult::Succeeded(snapshot_.state);
}

RenderControlResult FakeRenderer::flush(
    const core::render::RenderFlushRequest& request) noexcept {
    if (snapshot_.state == RenderLifecycleState::Closed
        || snapshot_.state == RenderLifecycleState::Failed) {
        record(RenderObservationOperation::Flush, RenderFailure::InvalidState);
        return RenderControlResult::Rejected(RenderFailure::InvalidState, snapshot_.state);
    }
    if (take_fault(RenderFaultKind::DeviceLost, RenderFaultTrigger::NextFlush)) {
        snapshot_.state = RenderLifecycleState::Failed;
        snapshot_.last_failure = RenderFailure::DeviceLost;
        record(RenderObservationOperation::Flush, RenderFailure::DeviceLost);
        return RenderControlResult::Failed(RenderFailure::DeviceLost);
    }

    snapshot_.buffered_frames = 0;
    snapshot_.generations = request.generations;
    snapshot_.last_failure = RenderFailure::None;
    if (snapshot_.state == RenderLifecycleState::Draining) {
        snapshot_.state = RenderLifecycleState::Started;
    }
    record(RenderObservationOperation::Flush);
    return RenderControlResult::Succeeded(snapshot_.state);
}

RenderControlResult FakeRenderer::drain(
    const core::render::RenderDrainRequest& request) noexcept {
    if (!request.is_valid()) {
        record(RenderObservationOperation::Drain, RenderFailure::Timeout);
        return RenderControlResult::Rejected(RenderFailure::Timeout, snapshot_.state);
    }
    if (snapshot_.state != RenderLifecycleState::Started
        && snapshot_.state != RenderLifecycleState::Draining) {
        record(RenderObservationOperation::Drain, RenderFailure::InvalidState);
        return RenderControlResult::Rejected(RenderFailure::InvalidState, snapshot_.state);
    }

    snapshot_.state = RenderLifecycleState::Draining;
    if (take_fault(RenderFaultKind::DrainTimeout, RenderFaultTrigger::NextDrain)) {
        snapshot_.last_failure = RenderFailure::Timeout;
        record(RenderObservationOperation::Drain, RenderFailure::Timeout);
        return RenderControlResult::TimedOut(snapshot_.state);
    }

    const auto drain_frames = snapshot_.buffered_frames;
    const auto required = duration_for_frames(drain_frames);
    if (request.timeout < required) {
        snapshot_.last_failure = RenderFailure::Timeout;
        record(RenderObservationOperation::Drain, RenderFailure::Timeout, drain_frames);
        return RenderControlResult::TimedOut(snapshot_.state);
    }

    clock_.advance(required);
    snapshot_.buffered_frames = 0;
    snapshot_.state = RenderLifecycleState::Stopped;
    snapshot_.last_failure = RenderFailure::None;
    record(RenderObservationOperation::Drain, RenderFailure::None, drain_frames, drain_frames);
    return RenderControlResult::Succeeded(snapshot_.state);
}

RenderControlResult FakeRenderer::reset(
    const core::render::RenderResetRequest& request) noexcept {
    if (snapshot_.state == RenderLifecycleState::Closed
        || !snapshot_.accepted_format.is_valid()) {
        record(RenderObservationOperation::Reset, RenderFailure::InvalidState);
        return RenderControlResult::Rejected(RenderFailure::InvalidState, snapshot_.state);
    }

    snapshot_.buffered_frames = 0;
    snapshot_.generations = request.generations;
    snapshot_.last_failure = RenderFailure::None;
    snapshot_.state = RenderLifecycleState::Open;
    record(RenderObservationOperation::Reset);
    return RenderControlResult::Succeeded(snapshot_.state);
}

RenderControlResult FakeRenderer::stop() noexcept {
    if (snapshot_.state == RenderLifecycleState::Stopped) {
        record(RenderObservationOperation::Stop);
        return RenderControlResult::NoOp(snapshot_.state);
    }
    if (snapshot_.state == RenderLifecycleState::Closed
        || snapshot_.state == RenderLifecycleState::Failed) {
        record(RenderObservationOperation::Stop, RenderFailure::InvalidState);
        return RenderControlResult::Rejected(RenderFailure::InvalidState, snapshot_.state);
    }

    snapshot_.state = RenderLifecycleState::Stopped;
    record(RenderObservationOperation::Stop);
    return RenderControlResult::Succeeded(snapshot_.state);
}

RenderControlResult FakeRenderer::close() noexcept {
    snapshot_ = {};
    for (auto& slot : faults_) {
        slot = {};
    }
    write_count_ = 0;
    consume_count_ = 0;
    record(RenderObservationOperation::Close);
    return RenderControlResult::Succeeded(snapshot_.state);
}

} // namespace kivo::testing::render
