#include "../state/playback_runtime_coordinator_state.hpp"

#include "kivo/core/playback/recovery/classification/decode_failure_domain.hpp"

namespace kivo::core::playback {

void PlaybackRuntimeCoordinator::Impl::handle_decode_failure(
    decode::DecodeFailure failure,
    uint64_t session_generation) noexcept {
    saturating_increment(decode_failure_events_);
    last_decode_failure_ = failure;
    last_decode_error_domain_ = classify_decode_failure(failure);

    const auto decision = session_.begin_recovery(
        session_generation,
        last_decode_error_domain_);
    if (!decision.recoverable) {
        static_cast<void>(session_.report_failure(
            session_generation,
            last_decode_error_domain_));
        return;
    }

    saturating_increment(recoverable_decode_failures_);
    if (decision.fallback == contract::RecoveryAction::StopMedia) {
        const auto stopped = renderer_.stop();
        if (stopped.is_success()) {
            producer_.reset();
            if (queue_) {
                queue_->reset();
            }
            if (session_.complete_recovery(
                    session_generation,
                    PlaybackRecoveryCompletion::Stopped)) {
                saturating_increment(decode_fallback_stops_);
                return;
            }
        }
    }

    if (!session_.complete_recovery(
            session_generation,
            PlaybackRecoveryCompletion::Failed)) {
        static_cast<void>(session_.report_failure(
            session_generation,
            last_decode_error_domain_));
    }
    saturating_increment(failed_decode_recoveries_);
}

} // namespace kivo::core::playback
