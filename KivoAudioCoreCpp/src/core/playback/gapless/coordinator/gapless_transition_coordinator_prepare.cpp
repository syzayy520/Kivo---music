#include "gapless_transition_coordinator_state.hpp"

#include <utility>

#include "kivo/core/playback/gapless/transition/gapless_transition_evaluator.hpp"

namespace kivo::core::playback {

std::unique_ptr<DecodeRenderQueueProducer> make_gapless_producer(
    render::SpscAudioBlockQueue& queue,
    const GaplessTrackPrepareRequest& request) noexcept {
    auto configuration = request.producer_configuration;
    configuration.timeline_origin_frames = request.plan.timeline_origin;
    configuration.end_of_stream_policy = request.plan.final_track
        ? QueueEndOfStreamPolicy::CloseAndMarkFinal
        : QueueEndOfStreamPolicy::KeepOpen;
    return DecodeRenderQueueProducer::create(
        *request.decoder,
        queue,
        request.plan.render_format,
        request.generations,
        request.decode_generation,
        configuration);
}

GaplessPrepareResult GaplessTransitionCoordinator::Impl::prepare_next(
    const GaplessTrackPrepareRequest& request) noexcept {
    const auto decision = evaluate_gapless_transition(
        current_plan_,
        request.plan);
    if (!request.is_valid()) {
        return {
            false,
            GaplessPrepareFailure::InvalidRequest,
            decision,
            decode::DecodeFailure::None};
    }
    if (current_plan_.final_track) {
        return {
            false,
            GaplessPrepareFailure::CurrentTrackFinal,
            decision,
            decode::DecodeFailure::None};
    }
    if (!decision.compatible) {
        return {
            false,
            GaplessPrepareFailure::TransitionRejected,
            decision,
            decode::DecodeFailure::None};
    }
    if (next_) {
        return {
            false,
            GaplessPrepareFailure::SlotOccupied,
            decision,
            decode::DecodeFailure::None};
    }
    auto candidate = make_gapless_producer(queue_, request);
    if (!candidate) {
        return {
            false,
            GaplessPrepareFailure::ProducerCreationFailed,
            decision,
            decode::DecodeFailure::InvalidBuffer};
    }
    const auto prefetched = candidate->prefetch();
    if (prefetched.disposition
        != DecodeRenderQueueProducerDisposition::Primed) {
        return {
            false,
            GaplessPrepareFailure::PrefetchFailed,
            decision,
            prefetched.decode_failure == decode::DecodeFailure::None
                ? decode::DecodeFailure::InvalidMediaData
                : prefetched.decode_failure};
    }
    next_plan_ = request.plan;
    next_ = std::move(candidate);
    refresh();
    return {
        true,
        GaplessPrepareFailure::None,
        decision,
        decode::DecodeFailure::None};
}

void GaplessTransitionCoordinator::Impl::refresh() noexcept {
    const auto current = current_->snapshot();
    snapshot_.decoded_frames = completed_decoded_;
    snapshot_.queued_frames = completed_queued_;
    saturating_add(snapshot_.decoded_frames, current.decoded_frames);
    saturating_add(snapshot_.queued_frames, current.queued_frames);
    snapshot_.next_track_prefetched = next_ != nullptr;
    if (next_) {
        saturating_add(
            snapshot_.decoded_frames,
            next_->snapshot().decoded_frames);
    }
}

} // namespace kivo::core::playback
