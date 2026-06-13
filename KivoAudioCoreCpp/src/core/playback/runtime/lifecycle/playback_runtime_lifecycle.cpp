#include "../result/playback_runtime_result_factory.hpp"
#include "../state/playback_runtime_coordinator_state.hpp"

namespace kivo::core::playback {

PlaybackRuntimeResult PlaybackRuntimeCoordinator::Impl::close_source(
    const PlaybackCommand& command) noexcept {
    const auto before = session_.snapshot();
    if (!active_
        || command.kind != contract::CommandKind::CloseSource
        || !matches_active_command(command, before)) {
        return reject_command(command);
    }
    const bool released = release_runtime();
    const auto command_result = session_.submit(command);
    if (!command_result.accepted()) {
        return runtime_result::rejected(
            PlaybackRuntimeFailure::CommandRejected,
            command_result);
    }
    if (!released) {
        saturating_increment(failed_operations_);
        return runtime_result::failed(
            PlaybackRuntimeFailure::BoundaryFailure,
            command_result);
    }
    return runtime_result::succeeded(command_result);
}

PlaybackRuntimeResult PlaybackRuntimeCoordinator::Impl::shutdown(
    const PlaybackCommand& command) noexcept {
    const auto before = session_.snapshot();
    const bool valid_generation =
        before.session_generation == 0
        || command.generation.generation == before.session_generation;
    if (command.kind != contract::CommandKind::Shutdown
        || !command.is_valid()
        || command.id.value <= before.last_command_id.value
        || !valid_generation) {
        return reject_command(command);
    }
    const bool released = release_runtime();
    const auto command_result = session_.submit(command);
    if (!command_result.accepted()) {
        return runtime_result::rejected(
            PlaybackRuntimeFailure::CommandRejected,
            command_result);
    }
    closed_ = true;
    if (!released) {
        saturating_increment(failed_operations_);
        return runtime_result::failed(
            PlaybackRuntimeFailure::BoundaryFailure,
            command_result);
    }
    return runtime_result::succeeded(command_result);
}

} // namespace kivo::core::playback
