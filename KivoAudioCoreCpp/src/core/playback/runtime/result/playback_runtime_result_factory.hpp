#pragma once

#include "kivo/core/playback/runtime/result/playback_runtime_result.hpp"

namespace kivo::core::playback::runtime_result {

[[nodiscard]] inline PlaybackRuntimeResult rejected(
    PlaybackRuntimeFailure failure,
    PlaybackCommandResult command = {}) noexcept {
    return {
        PlaybackRuntimeDisposition::Rejected,
        failure,
        command
    };
}

[[nodiscard]] inline PlaybackRuntimeResult failed(
    PlaybackRuntimeFailure failure,
    PlaybackCommandResult command = {},
    decode::DecodeFailure decode_failure = decode::DecodeFailure::None,
    render::RenderFailure render_failure =
        render::RenderFailure::None) noexcept {
    return {
        PlaybackRuntimeDisposition::Failed,
        failure,
        command,
        decode_failure,
        render_failure
    };
}

[[nodiscard]] inline PlaybackRuntimeResult succeeded(
    PlaybackCommandResult command) noexcept {
    return {
        PlaybackRuntimeDisposition::Succeeded,
        PlaybackRuntimeFailure::None,
        command
    };
}

} // namespace kivo::core::playback::runtime_result
