#include "../result/playback_runtime_result_factory.hpp"
#include "../state/playback_runtime_coordinator_state.hpp"

namespace kivo::core::playback {

PlaybackRuntimeResult PlaybackRuntimeCoordinator::Impl::resume(
    const PlaybackCommand& command) noexcept {
    const auto before = session_.snapshot();
    if (!active_
        || command.kind != contract::CommandKind::Resume
        || !matches_active_command(command, before)
        || (before.state != contract::CoreState::Ready
            && before.state != contract::CoreState::Paused
            && before.state != contract::CoreState::Playing)) {
        return reject_command(command);
    }
    const auto renderer_before = renderer_.snapshot().state;
    const auto control = renderer_.start();
    if (!control.is_success()) {
        return fail_active(
            PlaybackRuntimeFailure::RenderControlFailed,
            {},
            decode::DecodeFailure::None,
            control.failure());
    }
    const auto command_result = session_.submit(command);
    if (!command_result.accepted()) {
        if (renderer_before != render::RenderLifecycleState::Started) {
            static_cast<void>(renderer_.stop());
        }
        return runtime_result::rejected(
            PlaybackRuntimeFailure::CommandRejected,
            command_result);
    }
    return runtime_result::succeeded(command_result);
}

PlaybackRuntimeResult PlaybackRuntimeCoordinator::Impl::pause(
    const PlaybackCommand& command) noexcept {
    const auto before = session_.snapshot();
    if (!active_
        || command.kind != contract::CommandKind::Pause
        || !matches_active_command(command, before)
        || (before.state != contract::CoreState::Playing
            && before.state != contract::CoreState::Paused)) {
        return reject_command(command);
    }
    const auto renderer_before = renderer_.snapshot().state;
    const auto control = renderer_.stop();
    if (!control.is_success()) {
        return fail_active(
            PlaybackRuntimeFailure::RenderControlFailed,
            {},
            decode::DecodeFailure::None,
            control.failure());
    }
    const auto command_result = session_.submit(command);
    if (!command_result.accepted()) {
        if (renderer_before == render::RenderLifecycleState::Started) {
            static_cast<void>(renderer_.start());
        }
        return runtime_result::rejected(
            PlaybackRuntimeFailure::CommandRejected,
            command_result);
    }
    return runtime_result::succeeded(command_result);
}

PlaybackRuntimeResult PlaybackRuntimeCoordinator::Impl::stop(
    const PlaybackCommand& command) noexcept {
    const auto before = session_.snapshot();
    if (!active_
        || command.kind != contract::CommandKind::Stop
        || !matches_active_command(command, before)
        || before.state == contract::CoreState::Idle) {
        return reject_command(command);
    }
    const auto renderer_before = renderer_.snapshot().state;
    const auto control = renderer_.stop();
    if (!control.is_success()) {
        return fail_active(
            PlaybackRuntimeFailure::RenderControlFailed,
            {},
            decode::DecodeFailure::None,
            control.failure());
    }
    const auto command_result = session_.submit(command);
    if (!command_result.accepted()) {
        if (renderer_before == render::RenderLifecycleState::Started) {
            static_cast<void>(renderer_.start());
        }
        return runtime_result::rejected(
            PlaybackRuntimeFailure::CommandRejected,
            command_result);
    }
    return runtime_result::succeeded(command_result);
}

} // namespace kivo::core::playback
