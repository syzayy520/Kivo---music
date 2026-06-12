#include "../result/playback_runtime_result_factory.hpp"
#include "../state/playback_runtime_coordinator_state.hpp"

#include "kivo/core/playback/recovery/classification/decode_failure_domain.hpp"

#include <limits>
#include <utility>

namespace kivo::core::playback {

bool PlaybackRuntimeCoordinator::Impl::request_device_recovery(
    uint64_t session_generation) noexcept {
    saturating_increment(device_loss_events_);
    const auto decision = session_.begin_recovery(
        session_generation,
        contract::ErrorDomain::DeviceLost);
    if (!decision.recoverable
        || decision.primary != contract::RecoveryAction::ReopenDevice) {
        return false;
    }
    device_recovery_pending_ = true;
    return true;
}

PlaybackRuntimeResult PlaybackRuntimeCoordinator::Impl::recover() noexcept {
    std::scoped_lock lock{mutex_};
    if (closed_) {
        return runtime_result::rejected(
            PlaybackRuntimeFailure::RuntimeClosed);
    }
    const auto session_snapshot = session_.snapshot();
    if (!device_recovery_pending_
        || !active_
        || !queue_
        || session_snapshot.state != contract::CoreState::Recovering
        || session_snapshot.session_generation == 0) {
        return runtime_result::rejected(
            PlaybackRuntimeFailure::NoPendingRecovery);
    }

    device_recovery_pending_ = false;
    saturating_increment(device_recovery_attempts_);
    const auto generation = session_snapshot.session_generation;
    const auto fail = [this, generation](
        PlaybackRuntimeFailure failure,
        decode::DecodeFailure decode_failure =
            decode::DecodeFailure::None,
        render::RenderFailure render_failure =
            render::RenderFailure::None) noexcept {
        if (!session_.complete_recovery(
                generation,
                PlaybackRecoveryCompletion::Failed)) {
            static_cast<void>(session_.report_failure(generation));
        }
        if (decode_failure != decode::DecodeFailure::None) {
            static_cast<void>(session_.report_failure(
                generation,
                classify_decode_failure(decode_failure)));
        }
        saturating_increment(failed_operations_);
        saturating_increment(failed_device_recoveries_);
        return runtime_result::failed(
            failure,
            {},
            decode_failure,
            render_failure);
    };

    const auto maximum = std::numeric_limits<uint64_t>::max();
    if (decode_generation_.value() == maximum
        || generations_.seek.value() == maximum
        || generations_.flush.value() == maximum) {
        return fail(PlaybackRuntimeFailure::BoundaryFailure);
    }

    producer_.reset();
    queue_->reset();
    auto next_generations = generations_;
    next_generations.seek.id =
        contract::GenerationId::next(next_generations.seek.id);
    next_generations.flush.id =
        contract::GenerationId::next(next_generations.flush.id);
    next_generations.device = renderer_.capabilities().device_generation;
    const auto next_decode = decode::DecodeGeneration{
        contract::GenerationId::next(decode_generation_.id)};

    const auto render_close = renderer_.close();
    if (!render_close.is_success()) {
        return fail(
            PlaybackRuntimeFailure::RenderControlFailed,
            {},
            render_close.failure());
    }

    auto reopen_request = render_open_request_;
    reopen_request.requested_format = format_;
    reopen_request.format_policy =
        render::FormatAcceptancePolicy::Exact;
    reopen_request.generations = next_generations;
    const auto render_open = renderer_.open(reopen_request);
    if (!render_open.is_accepted()
        || render_open.accepted_format() != format_) {
        static_cast<void>(renderer_.close());
        return fail(
            PlaybackRuntimeFailure::RenderOpenFailed,
            {},
            render_open.is_accepted()
                ? render::RenderFailure::UnsupportedFormat
                : render_open.failure());
    }

    const auto decode_seek = decoder_.seek(
        session_snapshot.rendered_position,
        next_decode);
    if (!decode_seek.is_success()) {
        static_cast<void>(renderer_.close());
        return fail(
            PlaybackRuntimeFailure::DecodeControlFailed,
            decode_seek.failure);
    }

    const auto reopened_generations = renderer_.snapshot().generations;
    auto next_producer = create_producer(
        next_decode,
        reopened_generations);
    if (!next_producer) {
        static_cast<void>(renderer_.close());
        return fail(
            PlaybackRuntimeFailure::ProducerCreationFailed);
    }

    const auto render_start = renderer_.start();
    if (!render_start.is_success()) {
        static_cast<void>(renderer_.close());
        return fail(
            PlaybackRuntimeFailure::RenderControlFailed,
            {},
            render_start.failure());
    }
    if (!session_.complete_recovery(
            generation,
            PlaybackRecoveryCompletion::Restored)) {
        static_cast<void>(renderer_.close());
        static_cast<void>(session_.report_failure(generation));
        saturating_increment(failed_operations_);
        saturating_increment(failed_device_recoveries_);
        return runtime_result::failed(
            PlaybackRuntimeFailure::BoundaryFailure);
    }

    decode_generation_ = next_decode;
    generations_ = reopened_generations;
    render_open_request_ = reopen_request;
    render_open_request_.generations = reopened_generations;
    producer_ = std::move(next_producer);
    saturating_increment(successful_device_recoveries_);
    return runtime_result::succeeded();
}

} // namespace kivo::core::playback
