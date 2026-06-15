#include "host/abi/diagnostic/host_diagnostic_snapshot.hpp"

namespace kivo::host::abi {

core::observability::AudioDiagnosticSnapshot
make_host_diagnostic_snapshot(
    const core::playback::PlaybackRuntimeSnapshot& runtime,
    const platform::windows::wasapi::WasapiDiagnostics& wasapi) noexcept {
    core::observability::AudioDiagnosticSnapshot result{};
    result.render_underruns = wasapi.underrun_count;
    result.render_protocol_overruns = wasapi.overrun_count;
    result.queue_rejected_full = runtime.queue.rejected_full;
    result.queue_rejected_oversized = runtime.queue.rejected_oversized;
    result.producer_backpressure = runtime.producer.backpressure_count;
    result.device_invalidations = wasapi.device_invalidation_count;
    result.device_reopen_attempts = runtime.device_recovery_attempts;
    result.device_reopen_successes =
        runtime.successful_device_recoveries;
    result.device_reopen_failures = runtime.failed_device_recoveries;
    result.format_negotiation_attempts =
        runtime.format_negotiation_attempts;
    result.format_negotiation_successes =
        runtime.format_negotiation_successes;
    result.format_negotiation_failures =
        runtime.format_negotiation_failures;
    result.format_renegotiation_attempts =
        runtime.format_renegotiation_attempts;
    result.format_renegotiation_successes =
        runtime.format_renegotiation_successes;
    result.format_renegotiation_failures =
        runtime.format_renegotiation_failures;
    result.decode_failures_by_category =
        runtime.decode_failures_by_category;
    result.recovery_attempts = runtime.session.recovery_attempts;
    result.recovery_successes = runtime.session.recovery_successes;
    result.recovery_failures = runtime.session.recovery_failures;
    result.recovery_fallback_stops = runtime.decode_fallback_stops;
    result.successful_seeks = runtime.successful_seeks;
    result.drain_attempts = runtime.drain_attempts;
    result.successful_drains = runtime.successful_drains;
    result.failed_drains = runtime.failed_drains;
    result.drain_timeouts = runtime.drain_timeouts;
    result.stale_blocks_rejected = runtime.stale_blocks_rejected;
    result.stale_commands_rejected = runtime.session.stale_commands;
    result.source_replacements = runtime.session.source_replacements;
    return result;
}

} // namespace kivo::host::abi
