#include "../state/playback_runtime_coordinator_state.hpp"

#include "kivo/core/render/pump/render_pump.hpp"

namespace kivo::core::playback {

DecodeRenderQueueProducerResult
PlaybackRuntimeCoordinator::Impl::produce_step() noexcept {
    std::scoped_lock lock{mutex_};
    if (!active_ || !producer_) {
        return {
            DecodeRenderQueueProducerDisposition::Failed,
            decode::DecodeFailure::BoundaryFailure,
            render::SpscQueuePushResult::Closed,
            0
        };
    }
    const auto result = producer_->step();
    if (result.disposition
        == DecodeRenderQueueProducerDisposition::DiscardedStale) {
        saturating_increment(stale_blocks_rejected_);
        return result;
    }
    if (result.disposition
        == DecodeRenderQueueProducerDisposition::Failed) {
        static_cast<void>(session_.report_failure(
            session_.snapshot().session_generation));
        saturating_increment(failed_operations_);
    }
    return result;
}

render::RenderPumpResult
PlaybackRuntimeCoordinator::Impl::render_step() noexcept {
    std::scoped_lock lock{mutex_};
    const auto session_snapshot = session_.snapshot();
    if (!active_ || !queue_
        || session_snapshot.state != contract::CoreState::Playing) {
        return {
            render::RenderPumpDisposition::Rejected,
            render::RenderFailure::InvalidState
        };
    }

    const auto front = queue_->try_peek();
    render::RenderPump pump{*queue_, renderer_};
    const auto result = pump.step();
    bool position_reported = true;
    if (front
        && result.accepted_frames != 0
        && (result.disposition == render::RenderPumpDisposition::Progress
            || result.disposition
                == render::RenderPumpDisposition::EndOfStream)) {
        position_reported = session_.report_rendered_position(
            front->frame_offset + result.accepted_frames,
            session_snapshot.session_generation);
    }
    if (result.disposition == render::RenderPumpDisposition::EndOfStream) {
        if (!front || !position_reported) {
            static_cast<void>(session_.report_failure(
                session_snapshot.session_generation));
            saturating_increment(failed_operations_);
            return {
                render::RenderPumpDisposition::Failed,
                render::RenderFailure::BoundaryFailure,
                result.submitted_frames,
                result.accepted_frames
            };
        }
        return complete_end_of_stream(
            result,
            session_snapshot.session_generation);
    }
    if (result.disposition == render::RenderPumpDisposition::Failed
        || result.disposition == render::RenderPumpDisposition::Rejected) {
        static_cast<void>(session_.report_failure(
            session_snapshot.session_generation));
        saturating_increment(failed_operations_);
    }
    return result;
}

} // namespace kivo::core::playback
