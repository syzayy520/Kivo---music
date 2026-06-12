#include "../result/playback_runtime_result_factory.hpp"
#include "../state/playback_runtime_coordinator_state.hpp"

#include "kivo/core/playback/recovery/classification/decode_failure_domain.hpp"

#include <limits>

namespace kivo::core::playback {

PlaybackRuntimeResult PlaybackRuntimeCoordinator::Impl::fail_current(
    uint64_t session_generation,
    PlaybackRuntimeFailure failure) noexcept {
    if (session_generation != 0) {
        static_cast<void>(session_.report_failure(session_generation));
    }
    saturating_increment(failed_operations_);
    return runtime_result::failed(failure);
}

PlaybackRuntimeResult PlaybackRuntimeCoordinator::Impl::fail_active(
    PlaybackRuntimeFailure failure,
    PlaybackCommandResult command,
    decode::DecodeFailure decode_failure,
    render::RenderFailure render_failure) noexcept {
    const auto generation = session_.snapshot().session_generation;
    if (generation != 0) {
        static_cast<void>(session_.report_failure(
            generation,
            classify_decode_failure(decode_failure)));
    }
    saturating_increment(failed_operations_);
    return runtime_result::failed(
        failure,
        command,
        decode_failure,
        render_failure);
}

void PlaybackRuntimeCoordinator::Impl::rollback_open(
    bool replacing,
    uint64_t replaced_generation,
    contract::ErrorDomain domain) noexcept {
    saturating_increment(rolled_back_opens_);
    if (replacing && replaced_generation != 0) {
        static_cast<void>(session_.report_failure(
            replaced_generation,
            domain));
    }
}

void PlaybackRuntimeCoordinator::Impl::record_format_negotiation_attempt(
    bool renegotiation) noexcept {
    saturating_increment(format_negotiation_attempts_);
    if (renegotiation) {
        saturating_increment(format_renegotiation_attempts_);
    }
}

void PlaybackRuntimeCoordinator::Impl::record_format_negotiation_success(
    bool renegotiation) noexcept {
    saturating_increment(format_negotiation_successes_);
    if (renegotiation) {
        saturating_increment(format_renegotiation_successes_);
    }
}

void PlaybackRuntimeCoordinator::Impl::record_format_negotiation_failure(
    bool renegotiation) noexcept {
    saturating_increment(format_negotiation_failures_);
    if (renegotiation) {
        saturating_increment(format_renegotiation_failures_);
    }
}

bool PlaybackRuntimeCoordinator::Impl::release_runtime() noexcept {
    if (!active_) {
        return true;
    }
    producer_.reset();
    queue_.reset();
    const auto decode_result = decoder_.close();
    const auto render_result = renderer_.close();
    active_ = false;
    format_ = {};
    generations_ = {};
    decode_generation_ = {};
    render_open_request_ = {};
    drain_request_ = {};
    device_recovery_pending_ = false;
    return decode_result.is_success() && render_result.is_success();
}

std::unique_ptr<DecodeRenderQueueProducer>
PlaybackRuntimeCoordinator::Impl::create_producer(
    decode::DecodeGeneration decode_generation,
    render::RenderGenerationSet generations) noexcept {
    return DecodeRenderQueueProducer::create(
        decoder_,
        *queue_,
        format_,
        generations,
        decode_generation,
        producer_configuration_);
}

void PlaybackRuntimeCoordinator::Impl::saturating_increment(
    uint64_t& value) noexcept {
    if (value != std::numeric_limits<uint64_t>::max()) {
        ++value;
    }
}

} // namespace kivo::core::playback
