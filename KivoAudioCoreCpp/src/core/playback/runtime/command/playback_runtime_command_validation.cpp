#include "../state/playback_runtime_coordinator_state.hpp"

namespace kivo::core::playback {

bool PlaybackRuntimeCoordinator::Impl::can_open(
    const PlaybackCommand& command,
    const PlaybackSessionSnapshot& snapshot) const noexcept {
    return command.is_valid()
        && command.id.value > snapshot.last_command_id.value
        && snapshot.state != contract::CoreState::Closed
        && command.generation.generation != 0
        && command.generation.generation > snapshot.session_generation;
}

bool PlaybackRuntimeCoordinator::Impl::matches_active_command(
    const PlaybackCommand& command,
    const PlaybackSessionSnapshot& snapshot) const noexcept {
    return command.is_valid()
        && command.id.value > snapshot.last_command_id.value
        && snapshot.state != contract::CoreState::Closed
        && snapshot.session_generation != 0
        && command.generation.generation == snapshot.session_generation;
}

bool PlaybackRuntimeCoordinator::Impl::can_seek(
    const PlaybackCommand& command,
    const PlaybackSessionSnapshot& snapshot) const noexcept {
    return command.kind == contract::CommandKind::Seek
        && matches_active_command(command, snapshot)
        && command.requested_position != contract::kInvalidSamplePosition
        && (snapshot.state == contract::CoreState::Ready
            || snapshot.state == contract::CoreState::Playing
            || snapshot.state == contract::CoreState::Paused
            || snapshot.state == contract::CoreState::Seeking);
}

} // namespace kivo::core::playback
