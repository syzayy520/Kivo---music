#include "gapless_transition_coordinator_state.hpp"

#include <limits>
#include <utility>

namespace kivo::core::playback {

GaplessCoordinatorResult
GaplessTransitionCoordinator::Impl::step() noexcept {
    if (snapshot_.state == GaplessCoordinatorState::Failed) {
        return {
            GaplessCoordinatorDisposition::Failed,
            GaplessCoordinatorFailure::ProducerFailed,
            decode::DecodeFailure::BoundaryFailure,
            0,
            current_plan_.track_index};
    }
    if (snapshot_.state == GaplessCoordinatorState::EndOfStream) {
        return {
            GaplessCoordinatorDisposition::EndOfStream,
            GaplessCoordinatorFailure::None,
            decode::DecodeFailure::None,
            0,
            current_plan_.track_index};
    }
    snapshot_.state = GaplessCoordinatorState::Running;
    const auto result = current_->step();
    if (result.disposition
        != DecodeRenderQueueProducerDisposition::EndOfStream) {
        return map(result, GaplessCoordinatorDisposition::Queued);
    }
    if (current_plan_.final_track) {
        snapshot_.state = GaplessCoordinatorState::EndOfStream;
        return map(result, GaplessCoordinatorDisposition::EndOfStream);
    }
    if (!next_) {
        snapshot_.state = GaplessCoordinatorState::Failed;
        refresh();
        return {
            GaplessCoordinatorDisposition::Failed,
            GaplessCoordinatorFailure::MissingPrefetchedTrack,
            decode::DecodeFailure::None,
            0,
            current_plan_.track_index};
    }

    const auto completed = current_->snapshot();
    saturating_add(completed_decoded_, completed.decoded_frames);
    saturating_add(completed_queued_, completed.queued_frames);
    current_ = std::move(next_);
    current_plan_ = next_plan_;
    ++snapshot_.completed_transitions;
    snapshot_.current_track_index = current_plan_.track_index;
    const auto advanced = current_->step();
    if (advanced.disposition
        == DecodeRenderQueueProducerDisposition::Failed) {
        return map(advanced, GaplessCoordinatorDisposition::Failed);
    }
    if (advanced.disposition
            == DecodeRenderQueueProducerDisposition::EndOfStream
        && current_plan_.final_track) {
        snapshot_.state = GaplessCoordinatorState::EndOfStream;
    }
    return map(advanced, GaplessCoordinatorDisposition::TrackAdvanced);
}

GaplessCoordinatorResult GaplessTransitionCoordinator::Impl::map(
    const DecodeRenderQueueProducerResult& result,
    GaplessCoordinatorDisposition override_disposition) noexcept {
    auto disposition = override_disposition;
    if (result.disposition
        == DecodeRenderQueueProducerDisposition::Primed) {
        disposition = GaplessCoordinatorDisposition::Primed;
    } else if (result.disposition
        == DecodeRenderQueueProducerDisposition::Backpressure) {
        disposition = GaplessCoordinatorDisposition::Backpressure;
    } else if (result.disposition
        == DecodeRenderQueueProducerDisposition::Failed) {
        disposition = GaplessCoordinatorDisposition::Failed;
        snapshot_.state = GaplessCoordinatorState::Failed;
    }
    refresh();
    return {
        disposition,
        disposition == GaplessCoordinatorDisposition::Failed
            ? GaplessCoordinatorFailure::ProducerFailed
            : GaplessCoordinatorFailure::None,
        result.decode_failure,
        result.queued_frames,
        current_plan_.track_index};
}

void GaplessTransitionCoordinator::Impl::saturating_add(
    contract::FrameCount& target,
    contract::FrameCount amount) noexcept {
    const auto maximum = std::numeric_limits<contract::FrameCount>::max();
    target = amount > maximum - target ? maximum : target + amount;
}

} // namespace kivo::core::playback
