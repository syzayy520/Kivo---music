#include "host/abi/mapping/host_result_mapping.hpp"

namespace kivo::host::abi {

namespace {

[[nodiscard]] bool source_failure(
    core::decode::DecodeFailure failure) noexcept {
    using Failure = core::decode::DecodeFailure;
    return failure == Failure::SourceOpenFailed
        || failure == Failure::SourceReadFailed
        || failure == Failure::SourceSeekFailed
        || failure == Failure::UnsupportedSource;
}

} // namespace

kivo_audio_result map_runtime_result(
    const core::playback::PlaybackRuntimeResult& result) noexcept {
    using RuntimeFailure =
        core::playback::PlaybackRuntimeFailure;
    using CommandFailure =
        core::playback::PlaybackCommandFailure;
    if (result.succeeded()) {
        return KIVO_AUDIO_RESULT_OK;
    }
    if (result.failure == RuntimeFailure::RuntimeClosed) {
        return KIVO_AUDIO_RESULT_SHUTDOWN;
    }
    if (result.failure == RuntimeFailure::InvalidRequest) {
        return KIVO_AUDIO_RESULT_INVALID_ARGUMENT;
    }
    if (result.failure == RuntimeFailure::UnsupportedCommand) {
        return KIVO_AUDIO_RESULT_UNSUPPORTED_OPERATION;
    }
    if (result.command.failure == CommandFailure::OutOfOrderCommand
        || result.command.failure == CommandFailure::StaleSession) {
        return KIVO_AUDIO_RESULT_STALE_COMMAND;
    }
    if (result.decode_failure
        != core::decode::DecodeFailure::None) {
        return source_failure(result.decode_failure)
            ? KIVO_AUDIO_RESULT_SOURCE_FAILURE
            : KIVO_AUDIO_RESULT_DECODE_FAILURE;
    }
    if (result.render_failure
        != core::render::RenderFailure::None) {
        return KIVO_AUDIO_RESULT_RENDER_FAILURE;
    }
    if (result.failure == RuntimeFailure::CommandRejected
        || result.failure == RuntimeFailure::NoActiveRuntime) {
        return KIVO_AUDIO_RESULT_ILLEGAL_STATE;
    }
    return KIVO_AUDIO_RESULT_INTERNAL_FAILURE;
}

kivo_audio_state map_core_state(
    core::contract::CoreState state) noexcept {
    using State = core::contract::CoreState;
    switch (state) {
    case State::Idle: return KIVO_AUDIO_STATE_IDLE;
    case State::Ready: return KIVO_AUDIO_STATE_READY;
    case State::Playing: return KIVO_AUDIO_STATE_PLAYING;
    case State::Paused: return KIVO_AUDIO_STATE_PAUSED;
    case State::Seeking: return KIVO_AUDIO_STATE_SEEKING;
    case State::Draining: return KIVO_AUDIO_STATE_DRAINING;
    case State::Recovering: return KIVO_AUDIO_STATE_RECOVERING;
    case State::Stopped: return KIVO_AUDIO_STATE_STOPPED;
    case State::Failed: return KIVO_AUDIO_STATE_FAILED;
    case State::Closed: return KIVO_AUDIO_STATE_CLOSED;
    default: return KIVO_AUDIO_STATE_UNKNOWN;
    }
}

} // namespace kivo::host::abi
