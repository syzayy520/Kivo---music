#include "kivo/testing/render/renderer/fake_renderer.hpp"

#include <algorithm>
#include <limits>

namespace kivo::testing::render {

namespace {

using core::contract::FrameCount;

[[nodiscard]] FrameCount saturating_add(FrameCount left, FrameCount right) noexcept {
    const auto maximum = std::numeric_limits<FrameCount>::max();
    return right > maximum - left ? maximum : left + right;
}

} // namespace

using core::render::RenderFailure;
using core::render::RenderLifecycleState;
using core::render::RenderWriteResult;

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
        return RenderWriteResult::Rejected(
            block.frame_count,
            RenderFailure::UnsupportedFormat);
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

} // namespace kivo::testing::render
