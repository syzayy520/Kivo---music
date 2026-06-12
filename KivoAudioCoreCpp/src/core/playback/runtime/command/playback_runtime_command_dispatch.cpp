#include "../result/playback_runtime_result_factory.hpp"
#include "../state/playback_runtime_coordinator_state.hpp"

namespace kivo::core::playback {

PlaybackRuntimeResult PlaybackRuntimeCoordinator::Impl::execute(
    const PlaybackCommand& command) noexcept {
    std::scoped_lock lock{mutex_};
    if (closed_) {
        return runtime_result::rejected(
            PlaybackRuntimeFailure::RuntimeClosed);
    }
    switch (command.kind) {
    case contract::CommandKind::Resume:
        return resume(command);
    case contract::CommandKind::Pause:
        return pause(command);
    case contract::CommandKind::Stop:
        return stop(command);
    case contract::CommandKind::Flush:
        return flush(command);
    case contract::CommandKind::CloseSource:
        return close_source(command);
    case contract::CommandKind::Shutdown:
        return shutdown(command);
    default:
        return runtime_result::rejected(
            PlaybackRuntimeFailure::UnsupportedCommand);
    }
}

PlaybackRuntimeResult PlaybackRuntimeCoordinator::Impl::reject_command(
    const PlaybackCommand& command) noexcept {
    return runtime_result::rejected(
        active_
            ? PlaybackRuntimeFailure::CommandRejected
            : PlaybackRuntimeFailure::NoActiveRuntime,
        session_.submit(command));
}

} // namespace kivo::core::playback
