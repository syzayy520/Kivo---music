#include "kivo/testing/render/fake_renderer.hpp"

#include <algorithm>
#include <limits>

namespace kivo::testing::render {

namespace {

using core::contract::FrameCount;
using core::render::RenderControlResult;
using core::render::RenderFailure;
using core::render::RenderLifecycleState;
using core::render::RenderWriteResult;

[[nodiscard]] FrameCount saturating_add(FrameCount left, FrameCount right) noexcept {
    const auto maximum = std::numeric_limits<FrameCount>::max();
    return right > maximum - left ? maximum : left + right;
}

} // namespace

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
        return core::render::RenderOpenResult::Rejected(RenderFailure::StaleDeviceGeneration);
    }

    auto accepted_format = request.requested_format;
    if (request.requested_format != configuration_.supported_format) {
        if (request.format_policy == core::render::FormatAcceptancePolicy::Exact) {
            record(RenderObservationOperation::Open, RenderFailure::UnsupportedFormat);
            return core::render::RenderOpenResult::Rejected(RenderFailure::UnsupportedFormat);
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

core::render::RenderSnapshot FakeRenderer::snapshot() const noexcept {
    return snapshot_;
}

RenderWriteResult FakeRenderer::write(
    const core::render::AudioBlockView& block) noexcept {
    const auto ordinal = ++write_count_;
    if (snapshot_.state != RenderLifecycleState::Started) {
        record(
            RenderObservationOperation::Write,
            RenderFailure::InvalidState,
            block.frame_count,
            0,
            ordinal);
        return RenderWriteResult::Rejected(block.frame_count, RenderFailure::InvalidState);
    }
    if (!block.is_valid()) {
        record(
            RenderObservationOperation::Write,
            RenderFailure::InvalidBuffer,
            block.frame_count,
            0,
            ordinal);
        return RenderWriteResult::Rejected(block.frame_count, RenderFailure::InvalidBuffer);
    }
    if (block.format != snapshot_.accepted_format) {
        record(
            RenderObservationOperation::Write,
            RenderFailure::UnsupportedFormat,
            block.frame_count,
            0,
            ordinal);
        return RenderWriteResult::Rejected(block.frame_count, RenderFailure::UnsupportedFormat);
    }

    const auto stale_failure = generation_failure(block.generations);
    if (stale_failure != RenderFailure::None) {
        record(
            RenderObservationOperation::Write,
            stale_failure,
            block.frame_count,
            0,
            ordinal);
        return RenderWriteResult::Rejected(block.frame_count, stale_failure);
    }

    const auto scheduled_fault = take_write_fault(block, ordinal);
    if (scheduled_fault && scheduled_fault->kind == RenderFaultKind::DeviceLost) {
        snapshot_.state = RenderLifecycleState::Failed;
        snapshot_.last_failure = RenderFailure::DeviceLost;
        record(
            RenderObservationOperation::Write,
            RenderFailure::DeviceLost,
            block.frame_count,
            0,
            ordinal);
        return RenderWriteResult::Failed(block.frame_count, RenderFailure::DeviceLost);
    }

    const auto available_frames =
        snapshot_.buffer_capacity_frames - snapshot_.buffered_frames;
    const auto write_limit = std::min(
        available_frames,
        configuration_.capabilities.maximum_write_frames);
    auto accepted_frames = std::min(block.frame_count, write_limit);
    if (scheduled_fault && scheduled_fault->kind == RenderFaultKind::PartialWrite) {
        accepted_frames = std::min(accepted_frames, scheduled_fault->partial_write_frames);
    }

    if (accepted_frames == 0) {
        record(
            RenderObservationOperation::Write,
            RenderFailure::None,
            block.frame_count,
            0,
            ordinal);
        return RenderWriteResult::Delayed(
            block.frame_count,
            configuration_.delayed_write_retry);
    }

    snapshot_.buffered_frames += accepted_frames;
    snapshot_.total_accepted_frames =
        saturating_add(snapshot_.total_accepted_frames, accepted_frames);
    record(
        RenderObservationOperation::Write,
        RenderFailure::None,
        block.frame_count,
        accepted_frames,
        ordinal);

    return accepted_frames == block.frame_count
        ? RenderWriteResult::Full(block.frame_count)
        : RenderWriteResult::Partial(block.frame_count, accepted_frames);
}

bool FakeRenderer::schedule_fault(const ScheduledRenderFault& fault) noexcept {
    if (!fault.is_valid()) {
        return false;
    }
    for (auto& slot : faults_) {
        if (!slot.active) {
            slot.fault = fault;
            slot.active = true;
            record(RenderObservationOperation::FaultScheduled);
            return true;
        }
    }
    return false;
}

FakeConsumeResult FakeRenderer::consume(FrameCount requested_frames) noexcept {
    const auto ordinal = ++consume_count_;
    if (snapshot_.state != RenderLifecycleState::Started
        && snapshot_.state != RenderLifecycleState::Draining) {
        record(
            RenderObservationOperation::Consume,
            RenderFailure::InvalidState,
            requested_frames,
            0,
            ordinal);
        return {requested_frames, 0, false};
    }

    const auto forced_underrun =
        take_fault(RenderFaultKind::ForcedUnderrun, RenderFaultTrigger::NextConsume);
    const auto consumed_frames = std::min(requested_frames, snapshot_.buffered_frames);
    const auto underrun = forced_underrun || requested_frames > snapshot_.buffered_frames;
    snapshot_.buffered_frames -= consumed_frames;
    clock_.advance(duration_for_frames(consumed_frames));

    if (underrun) {
        snapshot_.last_failure = RenderFailure::Underrun;
    }
    if (snapshot_.state == RenderLifecycleState::Draining
        && snapshot_.buffered_frames == 0) {
        snapshot_.state = RenderLifecycleState::Stopped;
    }

    record(
        RenderObservationOperation::Consume,
        underrun ? RenderFailure::Underrun : RenderFailure::None,
        requested_frames,
        consumed_frames,
        ordinal);
    return {requested_frames, consumed_frames, underrun};
}

size_t FakeRenderer::observation_count() const noexcept {
    return observations_.size();
}

std::optional<RenderObservation> FakeRenderer::observation_at(size_t index) const noexcept {
    return observations_.at(index);
}

void FakeRenderer::clear_observations() noexcept {
    observations_.clear();
}

uint64_t FakeRenderer::write_count() const noexcept {
    return write_count_;
}

uint64_t FakeRenderer::consume_count() const noexcept {
    return consume_count_;
}

RenderFailure FakeRenderer::generation_failure(
    const core::render::RenderGenerationSet& generations) const noexcept {
    if (generations.stream != snapshot_.generations.stream) {
        return RenderFailure::StaleStreamGeneration;
    }
    if (generations.seek != snapshot_.generations.seek) {
        return RenderFailure::StaleSeekGeneration;
    }
    if (generations.flush != snapshot_.generations.flush) {
        return RenderFailure::StaleFlushGeneration;
    }
    if (generations.device != snapshot_.generations.device) {
        return RenderFailure::StaleDeviceGeneration;
    }
    return RenderFailure::None;
}

std::optional<ScheduledRenderFault> FakeRenderer::take_write_fault(
    const core::render::AudioBlockView& block,
    uint64_t write_ordinal) noexcept {
    for (auto& slot : faults_) {
        if (!slot.active) {
            continue;
        }

        const auto& fault = slot.fault;
        const auto matches =
            fault.trigger == RenderFaultTrigger::NextWrite
            || (fault.trigger == RenderFaultTrigger::WriteCount
                && write_ordinal >= fault.trigger_value)
            || (fault.trigger == RenderFaultTrigger::FramePositionAtOrAfter
                && block.frame_offset >= fault.trigger_value)
            || (fault.trigger == RenderFaultTrigger::StreamGeneration
                && block.generations.stream.value() == fault.trigger_value);
        if (matches) {
            slot.active = false;
            return fault;
        }
    }
    return std::nullopt;
}

bool FakeRenderer::take_fault(
    RenderFaultKind kind,
    RenderFaultTrigger trigger) noexcept {
    for (auto& slot : faults_) {
        if (slot.active && slot.fault.kind == kind && slot.fault.trigger == trigger) {
            slot.active = false;
            return true;
        }
    }
    return false;
}

std::chrono::nanoseconds FakeRenderer::duration_for_frames(FrameCount frames) const noexcept {
    const auto sample_rate = snapshot_.accepted_format.format.sample_rate;
    if (sample_rate == 0 || frames == 0) {
        return std::chrono::nanoseconds{0};
    }

    constexpr uint64_t nanoseconds_per_second = 1'000'000'000;
    const auto whole_seconds = frames / sample_rate;
    const auto remaining_frames = frames % sample_rate;
    const auto maximum = static_cast<uint64_t>(std::numeric_limits<int64_t>::max());
    if (whole_seconds > maximum / nanoseconds_per_second) {
        return std::chrono::nanoseconds::max();
    }

    const auto whole_nanoseconds = whole_seconds * nanoseconds_per_second;
    const auto fractional_nanoseconds =
        remaining_frames * nanoseconds_per_second / sample_rate;
    if (fractional_nanoseconds > maximum - whole_nanoseconds) {
        return std::chrono::nanoseconds::max();
    }
    return std::chrono::nanoseconds{
        static_cast<int64_t>(whole_nanoseconds + fractional_nanoseconds)};
}

void FakeRenderer::record(
    RenderObservationOperation operation,
    RenderFailure failure,
    FrameCount submitted_frames,
    FrameCount accepted_frames,
    uint64_t ordinal) noexcept {
    observations_.push({
        operation,
        clock_.now().time_since_epoch(),
        snapshot_.state,
        failure,
        submitted_frames,
        accepted_frames,
        ordinal,
        snapshot_.generations
    });
}

} // namespace kivo::testing::render
