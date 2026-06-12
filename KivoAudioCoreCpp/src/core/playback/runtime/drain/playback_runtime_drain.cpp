#include "../state/playback_runtime_coordinator_state.hpp"

namespace kivo::core::playback {

render::RenderPumpResult
PlaybackRuntimeCoordinator::Impl::complete_end_of_stream(
    render::RenderPumpResult result,
    uint64_t session_generation) noexcept {
    saturating_increment(drain_attempts_);
    const auto ticket = session_.begin_drain(session_generation);
    if (!ticket.is_valid()) {
        static_cast<void>(session_.report_failure(session_generation));
        saturating_increment(failed_operations_);
        saturating_increment(failed_drains_);
        result.disposition = render::RenderPumpDisposition::Failed;
        result.failure = render::RenderFailure::BoundaryFailure;
        return result;
    }

    const auto control = renderer_.drain(drain_request_);
    const bool renderer_stopped =
        control.is_success()
        && control.state() == render::RenderLifecycleState::Stopped;
    if (!renderer_stopped) {
        if (!session_.complete_drain(
                ticket,
                PlaybackDrainCompletion::Failed)) {
            static_cast<void>(session_.report_failure(
                session_generation));
        }
        saturating_increment(failed_operations_);
        saturating_increment(failed_drains_);
        if (control.failure() == render::RenderFailure::Timeout) {
            saturating_increment(drain_timeouts_);
        }
        result.disposition = render::RenderPumpDisposition::Failed;
        result.failure = control.is_success()
            ? render::RenderFailure::BoundaryFailure
            : control.failure();
        return result;
    }

    if (!session_.complete_drain(
            ticket,
            PlaybackDrainCompletion::Succeeded)) {
        static_cast<void>(session_.report_failure(session_generation));
        saturating_increment(failed_operations_);
        saturating_increment(failed_drains_);
        result.disposition = render::RenderPumpDisposition::Failed;
        result.failure = render::RenderFailure::BoundaryFailure;
        return result;
    }

    saturating_increment(successful_drains_);
    return result;
}

} // namespace kivo::core::playback
