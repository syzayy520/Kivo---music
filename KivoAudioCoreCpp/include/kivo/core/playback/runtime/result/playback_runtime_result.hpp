#pragma once

#include <cstdint>

#include "kivo/core/decode/failure/decode_failure.hpp"
#include "kivo/core/playback/runtime/failure/playback_runtime_failure.hpp"
#include "kivo/core/playback/session/playback_command_result.hpp"
#include "kivo/core/render/failure/render_failure.hpp"

namespace kivo::core::playback {

enum class PlaybackRuntimeDisposition : uint8_t {
    Succeeded = 0,
    Rejected,
    Failed
};

struct PlaybackRuntimeResult {
    PlaybackRuntimeDisposition disposition{
        PlaybackRuntimeDisposition::Failed};
    PlaybackRuntimeFailure failure{
        PlaybackRuntimeFailure::BoundaryFailure};
    PlaybackCommandResult command{};
    decode::DecodeFailure decode_failure{
        decode::DecodeFailure::None};
    render::RenderFailure render_failure{
        render::RenderFailure::None};

    [[nodiscard]] constexpr bool succeeded() const noexcept {
        return disposition == PlaybackRuntimeDisposition::Succeeded;
    }

    [[nodiscard]] constexpr bool operator==(
        const PlaybackRuntimeResult&) const noexcept = default;
};

} // namespace kivo::core::playback
