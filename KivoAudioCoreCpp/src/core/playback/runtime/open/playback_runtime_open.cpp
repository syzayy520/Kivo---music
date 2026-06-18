#include "../result/playback_runtime_result_factory.hpp"
#include "../state/playback_runtime_coordinator_state.hpp"

#include "kivo/core/playback/recovery/classification/decode_failure_domain.hpp"

#include <utility>

namespace kivo::core::playback {

PlaybackRuntimeResult PlaybackRuntimeCoordinator::Impl::open(
    const PlaybackCommand& command,
    std::unique_ptr<decode::ByteSourceBoundary> source,
    const PlaybackRuntimeOpenRequest& request) noexcept {
    std::scoped_lock lock{mutex_};
    if (closed_) {
        return runtime_result::rejected(
            PlaybackRuntimeFailure::RuntimeClosed);
    }
    if (command.kind != contract::CommandKind::OpenSource
        || !source
        || !request.is_valid()
        || source->generation().value() == 0) {
        return runtime_result::rejected(
            PlaybackRuntimeFailure::InvalidRequest);
    }

    const auto before = session_.snapshot();
    if (!can_open(command, before)) {
        return runtime_result::rejected(
            PlaybackRuntimeFailure::CommandRejected,
            session_.submit(command));
    }

    auto staged_queue = render::SpscAudioBlockQueue::create(
        request.queue_configuration);
    if (!staged_queue) {
        return fail_current(
            before.session_generation,
            PlaybackRuntimeFailure::QueueCreationFailed);
    }

    const bool replacing = active_;
    const auto replaced_generation = before.session_generation;
    if (replacing && !release_runtime()) {
        return fail_current(
            replaced_generation,
            PlaybackRuntimeFailure::BoundaryFailure);
    }

    auto render_request = request.render_request;
    render_request.generations.stream.id = source->generation().id;
    record_format_negotiation_attempt(replacing);
    const auto render_open = renderer_.open(render_request);
    if (!render_open.is_accepted()) {
        record_format_negotiation_failure(replacing);
        rollback_open(replacing, replaced_generation);
        return runtime_result::failed(
            PlaybackRuntimeFailure::RenderOpenFailed,
            {},
            decode::DecodeFailure::None,
            render_open.failure());
    }
    record_format_negotiation_success(replacing);

    const decode::DecodeOpenRequest decode_request{
        render_open.accepted_format(),
        request.decode_generation,
        request.allow_conversion,
        request.resample_quality,
        request.conversion_dither
    };
    const auto decode_open = decoder_.open(
        std::move(source),
        decode_request);
    if (!decode_open.is_accepted()) {
        record_decode_failure(decode_open.failure());
        static_cast<void>(decoder_.close());
        static_cast<void>(renderer_.close());
        rollback_open(
            replacing,
            replaced_generation,
            classify_decode_failure(decode_open.failure()));
        return runtime_result::failed(
            PlaybackRuntimeFailure::DecodeOpenFailed,
            {},
            decode_open.failure());
    }

    const bool stale_source =
        decode_open.probe().source_generation.id
        != render_request.generations.stream.id;
    const bool stale_decode =
        decode_open.probe().decode_generation
        != request.decode_generation;
    const bool wrong_format =
        decode_open.probe().output_format
        != render_open.accepted_format();
    if (stale_source || stale_decode || wrong_format) {
        static_cast<void>(decoder_.close());
        static_cast<void>(renderer_.close());
        const auto decode_failure =
            stale_source
                ? decode::DecodeFailure::StaleSourceGeneration
                : stale_decode
                    ? decode::DecodeFailure::StaleDecodeGeneration
                    : decode::DecodeFailure::BoundaryFailure;
        record_decode_failure(decode_failure);
        rollback_open(
            replacing,
            replaced_generation,
            classify_decode_failure(decode_failure));
        return runtime_result::failed(
            PlaybackRuntimeFailure::DecodeOpenFailed,
            {},
            decode_failure);
    }

    const auto generations = renderer_.snapshot().generations;
    auto staged_producer = DecodeRenderQueueProducer::create(
        decoder_,
        *staged_queue,
        render_open.accepted_format(),
        generations,
        request.decode_generation,
        request.producer_configuration);
    if (!staged_producer) {
        static_cast<void>(decoder_.close());
        static_cast<void>(renderer_.close());
        rollback_open(
            replacing,
            replaced_generation,
            contract::ErrorDomain::InternalBug);
        return runtime_result::failed(
            PlaybackRuntimeFailure::ProducerCreationFailed);
    }

    const auto command_result = session_.submit(command);
    if (!command_result.accepted()) {
        static_cast<void>(decoder_.close());
        static_cast<void>(renderer_.close());
        rollback_open(replacing, replaced_generation);
        return runtime_result::rejected(
            PlaybackRuntimeFailure::CommandRejected,
            command_result);
    }

    queue_ = std::move(staged_queue);
    producer_ = std::move(staged_producer);
    format_ = render_open.accepted_format();
    // P1-3/4: capture the real total duration + source rate from the validated
    // probe so the host can render true time/rate (the ABI snapshot grows a tail
    // for these in step 5). Written under the same lock as format_; read only via
    // snapshot() under the same mutex_ — never on the realtime render path.
    {
        const auto& probe = decode_open.probe();
        total_frames_ = probe.duration_frames;
        total_frames_known_ = probe.duration_known;
        source_sample_rate_ = probe.resample.source_rate;
        resample_active_ =
            probe.resample.source_rate != probe.resample.target_rate;
        // P2 bit-perfect-evidence inputs. device_format == render format is
        // valid ONLY because render_open_request_.format_policy is Exact (set
        // below): the renderer negotiated the device against the exact requested
        // format, so the accepted format IS the device-delivered format. This is
        // NOT an independent device measurement. requested==actual mode because
        // the renderer accepted the requested mode (no separate downgrade signal
        // exists yet). policy_allows_bit_perfect is a CONSERVATIVE proxy: only
        // true when Exclusive was requested, so it can never falsely assert
        // bit-perfect — it can only gate it off.
        source_format_ = probe.native_format.format;
        device_format_ = format_.format;
        conversion_snapshot_ = probe.conversion_snapshot;
        requested_output_mode_ = render_request.output_mode;
        actual_output_mode_ = render_request.output_mode;
        policy_allows_bit_perfect_ =
            render_request.output_mode == render::RenderOutputMode::Exclusive;
    }
    generations_ = generations;
    decode_generation_ = request.decode_generation;
    queue_configuration_ = request.queue_configuration;
    producer_configuration_ = request.producer_configuration;
    render_open_request_ = render_request;
    render_open_request_.requested_format = format_;
    render_open_request_.format_policy =
        render::FormatAcceptancePolicy::Exact;
    drain_request_ = request.drain_request;
    device_recovery_pending_ = false;
    active_ = true;
    saturating_increment(successful_opens_);
    return runtime_result::succeeded(command_result);
}

} // namespace kivo::core::playback
