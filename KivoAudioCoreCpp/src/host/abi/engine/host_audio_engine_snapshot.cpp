#include "host/abi/engine/host_audio_engine.hpp"

#include <algorithm>
#include <cstring>

#include "host/abi/diagnostic/host_diagnostic_snapshot.hpp"
#include "host/abi/mapping/host_result_mapping.hpp"

namespace kivo::host::abi {

kivo_audio_result HostAudioEngine::snapshot(
    void* snapshot,
    uint32_t snapshot_size) noexcept {
    std::scoped_lock lock{command_mutex_};
    const auto current = runtime_.snapshot();
    const auto diagnostics =
        make_host_diagnostic_snapshot(current, renderer_.diagnostics());
    kivo_audio_snapshot_v1 result{};
    result.struct_size = sizeof(result);
    result.struct_version = KIVO_AUDIO_ABI_STRUCT_VERSION_1;
    result.state =
        static_cast<uint32_t>(map_core_state(current.session.state));
    result.active = current.active ? 1u : 0u;
    result.session_generation = current.session.session_generation;
    result.last_command_id = current.session.last_command_id.value;
    result.rendered_position_frames = current.session.rendered_position;
    result.decoded_frames = current.producer.decoded_frames;
    result.queued_frames = current.queue.queued_frames;
    result.successful_opens = current.successful_opens;
    result.successful_seeks = current.successful_seeks;
    result.failed_operations = current.failed_operations;
    result.stale_blocks_rejected = current.stale_blocks_rejected;
    result.device_loss_events = current.device_loss_events;
    result.decode_failure_events = current.decode_failure_events;
    result.last_result = last_result_;
    result.render_underrun_events = diagnostics.render_underruns;
    result.render_protocol_overrun_events =
        diagnostics.render_protocol_overruns;
    result.queue_full_rejections = diagnostics.queue_rejected_full;
    result.queue_oversized_rejections =
        diagnostics.queue_rejected_oversized;
    result.producer_backpressure_events =
        diagnostics.producer_backpressure;
    result.device_invalidation_events =
        diagnostics.device_invalidations;
    result.device_reopen_attempts = diagnostics.device_reopen_attempts;
    result.device_reopen_successes = diagnostics.device_reopen_successes;
    result.device_reopen_failures = diagnostics.device_reopen_failures;
    result.format_negotiation_attempts =
        diagnostics.format_negotiation_attempts;
    result.format_negotiation_successes =
        diagnostics.format_negotiation_successes;
    result.format_negotiation_failures =
        diagnostics.format_negotiation_failures;
    result.format_renegotiation_attempts =
        diagnostics.format_renegotiation_attempts;
    result.format_renegotiation_successes =
        diagnostics.format_renegotiation_successes;
    result.format_renegotiation_failures =
        diagnostics.format_renegotiation_failures;
    for (std::size_t index = 0;
         index < diagnostics.decode_failures_by_category.size();
         ++index) {
        result.decode_failures_by_category[index] =
            diagnostics.decode_failures_by_category[index];
    }
    result.recovery_attempts = diagnostics.recovery_attempts;
    result.recovery_successes = diagnostics.recovery_successes;
    result.recovery_failures = diagnostics.recovery_failures;
    result.recovery_fallback_stops =
        diagnostics.recovery_fallback_stops;
    result.drain_attempts = diagnostics.drain_attempts;
    result.successful_drains = diagnostics.successful_drains;
    result.failed_drains = diagnostics.failed_drains;
    result.drain_timeouts = diagnostics.drain_timeouts;
    result.stale_commands_rejected =
        diagnostics.stale_commands_rejected;
    result.source_replacements = diagnostics.source_replacements;
    // ABI 1.1.0 timebase tail (gated host-side by PLAYBACK_TIMEBASE_SNAPSHOT).
    result.render_sample_rate = current.format.format.sample_rate;
    result.total_frames_known = current.total_frames_known ? 1u : 0u;
    result.total_frames = current.total_frames;
    result.source_sample_rate = current.source_sample_rate;
    result.resample_active = current.resample_active ? 1u : 0u;

    std::memcpy(
        snapshot,
        &result,
        std::min<std::size_t>(snapshot_size, sizeof(result)));
    return KIVO_AUDIO_RESULT_OK;
}

} // namespace kivo::host::abi
