#include "../result/playback_runtime_result_factory.hpp"
#include "../state/playback_runtime_coordinator_state.hpp"

#include "kivo/core/playback/recovery/classification/decode_failure_domain.hpp"

namespace kivo::core::playback {

PlaybackRuntimeResult PlaybackRuntimeCoordinator::Impl::seek(
    const PlaybackCommand& command) noexcept {
    std::scoped_lock lock{mutex_};
    if (closed_) {
        return runtime_result::rejected(
            PlaybackRuntimeFailure::RuntimeClosed);
    }
    const auto before = session_.snapshot();
    if (!active_ || !can_seek(command, before)) {
        return runtime_result::rejected(
            active_
                ? PlaybackRuntimeFailure::CommandRejected
                : PlaybackRuntimeFailure::NoActiveRuntime,
            session_.submit(command));
    }

    const auto command_result = session_.submit(command);
    if (!command_result.accepted()) {
        return runtime_result::rejected(
            PlaybackRuntimeFailure::CommandRejected,
            command_result);
    }

    producer_.reset();
    const auto next_decode = decode::DecodeGeneration{
        contract::GenerationId::next(decode_generation_.id)};
    auto next_generations = generations_;
    next_generations.seek.id =
        contract::GenerationId::next(next_generations.seek.id);
    next_generations.flush.id =
        contract::GenerationId::next(next_generations.flush.id);

    const auto decode_result = decoder_.seek(
        command.requested_position,
        next_decode);
    queue_->reset();
    const auto render_result = renderer_.flush({next_generations});
    if (!decode_result.is_success() || !render_result.is_success()) {
        static_cast<void>(session_.complete_seek(
            before.session_generation,
            false));
        if (!decode_result.is_success()) {
            static_cast<void>(session_.report_failure(
                before.session_generation,
                classify_decode_failure(decode_result.failure)));
        }
        saturating_increment(failed_operations_);
        return runtime_result::failed(
            !decode_result.is_success()
                ? PlaybackRuntimeFailure::DecodeControlFailed
                : PlaybackRuntimeFailure::RenderControlFailed,
            command_result,
            decode_result.failure,
            render_result.failure());
    }

    auto next_producer = create_producer(
        next_decode,
        renderer_.snapshot().generations);
    if (!next_producer) {
        static_cast<void>(session_.complete_seek(
            before.session_generation,
            false));
        saturating_increment(failed_operations_);
        return runtime_result::failed(
            PlaybackRuntimeFailure::ProducerCreationFailed);
    }

    decode_generation_ = next_decode;
    generations_ = renderer_.snapshot().generations;
    producer_ = std::move(next_producer);
    if (!session_.complete_seek(before.session_generation, true)) {
        static_cast<void>(session_.report_failure(
            before.session_generation));
        saturating_increment(failed_operations_);
        return runtime_result::failed(
            PlaybackRuntimeFailure::BoundaryFailure);
    }
    saturating_increment(successful_seeks_);
    return runtime_result::succeeded(command_result);
}

} // namespace kivo::core::playback
