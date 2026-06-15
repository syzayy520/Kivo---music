#include "../result/playback_runtime_result_factory.hpp"
#include "../state/playback_runtime_coordinator_state.hpp"

namespace kivo::core::playback {

PlaybackRuntimeResult PlaybackRuntimeCoordinator::Impl::flush(
    const PlaybackCommand& command) noexcept {
    const auto before = session_.snapshot();
    if (!active_
        || command.kind != contract::CommandKind::Flush
        || !matches_active_command(command, before)
        || (before.state != contract::CoreState::Ready
            && before.state != contract::CoreState::Playing
            && before.state != contract::CoreState::Paused
            && before.state != contract::CoreState::Seeking)) {
        return reject_command(command);
    }

    producer_.reset();
    const auto next_decode = decode::DecodeGeneration{
        contract::GenerationId::next(decode_generation_.id)};
    auto next_generations = generations_;
    next_generations.flush.id =
        contract::GenerationId::next(next_generations.flush.id);
    const auto decode_result = decoder_.flush(next_decode);
    queue_->reset();
    const auto render_result = renderer_.flush({next_generations});
    if (!decode_result.is_success() || !render_result.is_success()) {
        if (decode_result.is_success()
            && render_result.failure() == render::RenderFailure::DeviceLost
            && request_device_recovery(before.session_generation)) {
            saturating_increment(failed_operations_);
            return runtime_result::failed(
                PlaybackRuntimeFailure::RenderControlFailed,
                {},
                decode::DecodeFailure::None,
                render_result.failure());
        }
        return fail_active(
            !decode_result.is_success()
                ? PlaybackRuntimeFailure::DecodeControlFailed
                : PlaybackRuntimeFailure::RenderControlFailed,
            {},
            decode_result.failure,
            render_result.failure());
    }

    auto next_producer = create_producer(
        next_decode,
        renderer_.snapshot().generations);
    if (!next_producer) {
        return fail_active(
            PlaybackRuntimeFailure::ProducerCreationFailed);
    }
    const auto command_result = session_.submit(command);
    if (!command_result.accepted()) {
        return fail_active(
            PlaybackRuntimeFailure::CommandRejected,
            command_result);
    }

    decode_generation_ = next_decode;
    generations_ = renderer_.snapshot().generations;
    producer_ = std::move(next_producer);
    return runtime_result::succeeded(command_result);
}

} // namespace kivo::core::playback
