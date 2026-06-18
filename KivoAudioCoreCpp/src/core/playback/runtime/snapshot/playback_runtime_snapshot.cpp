#include "../state/playback_runtime_coordinator_state.hpp"

#include "kivo/core/output/truth/evidence/bit_perfect_evidence_builder.hpp"
#include "kivo/core/output/truth/evaluation/bit_perfect_truth_evaluator.hpp"

namespace kivo::core::playback {

PlaybackRuntimeSnapshot
PlaybackRuntimeCoordinator::Impl::snapshot() const noexcept {
    std::scoped_lock lock{mutex_};
    PlaybackRuntimeSnapshot result{};
    result.active = active_;
    result.format = format_;
    result.total_frames = total_frames_;
    result.total_frames_known = total_frames_known_;
    result.source_sample_rate = source_sample_rate_;
    result.resample_active = resample_active_;
    // P2 audio-truth: build the (previously dead) evidence + evaluate the verdict
    // here, under mutex_, off the realtime thread. Only when active — an idle/
    // released runtime leaves both default, so is_complete()==false and the host
    // reports EvidenceIncomplete (never a stale verdict). Both calls are
    // noexcept/alloc-free.
    if (active_) {
        result.bit_perfect_evidence = output::build_bit_perfect_evidence(
            requested_output_mode_,
            actual_output_mode_,
            source_format_,
            format_.format,
            device_format_,
            /*host_audio_engine_in_path=*/false,
            policy_allows_bit_perfect_,
            conversion_snapshot_,
            producer_ ? producer_->snapshot().processing
                      : processing::AudioProcessingSnapshot{});
        result.bit_perfect_report =
            output::evaluate_bit_perfect_truth(result.bit_perfect_evidence)
                .report;
    }
    result.decode_generation = decode_generation_;
    result.render_generations = generations_;
    result.session = session_.snapshot();
    result.renderer = renderer_.snapshot();
    if (queue_) {
        result.queue = queue_->snapshot();
    }
    if (producer_) {
        result.producer = producer_->snapshot();
    }
    result.successful_opens = successful_opens_;
    result.rolled_back_opens = rolled_back_opens_;
    result.successful_seeks = successful_seeks_;
    result.failed_operations = failed_operations_;
    result.stale_blocks_rejected = stale_blocks_rejected_;
    result.drain_attempts = drain_attempts_;
    result.successful_drains = successful_drains_;
    result.failed_drains = failed_drains_;
    result.drain_timeouts = drain_timeouts_;
    result.device_recovery_pending = device_recovery_pending_;
    result.device_loss_events = device_loss_events_;
    result.device_recovery_attempts = device_recovery_attempts_;
    result.successful_device_recoveries =
        successful_device_recoveries_;
    result.failed_device_recoveries = failed_device_recoveries_;
    result.decode_failure_events = decode_failure_events_;
    result.recoverable_decode_failures =
        recoverable_decode_failures_;
    result.decode_fallback_stops = decode_fallback_stops_;
    result.failed_decode_recoveries =
        failed_decode_recoveries_;
    result.format_negotiation_attempts =
        format_negotiation_attempts_;
    result.format_negotiation_successes =
        format_negotiation_successes_;
    result.format_negotiation_failures =
        format_negotiation_failures_;
    result.format_renegotiation_attempts =
        format_renegotiation_attempts_;
    result.format_renegotiation_successes =
        format_renegotiation_successes_;
    result.format_renegotiation_failures =
        format_renegotiation_failures_;
    result.decode_failures_by_category =
        decode_failures_by_category_;
    result.last_decode_failure = last_decode_failure_;
    result.last_decode_error_domain =
        last_decode_error_domain_;
    return result;
}

} // namespace kivo::core::playback
