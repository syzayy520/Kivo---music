#pragma once

#include <memory>

#include "kivo/core/decode/boundary/audio_decode_boundary.hpp"
#include "kivo/core/playback/pipeline/decode_render_queue_producer_result.hpp"
#include "kivo/core/playback/runtime/request/playback_runtime_open_request.hpp"
#include "kivo/core/playback/runtime/result/playback_runtime_result.hpp"
#include "kivo/core/playback/runtime/snapshot/playback_runtime_snapshot.hpp"
#include "kivo/core/playback/session/command/playback_command.hpp"
#include "kivo/core/playback/session/controller/playback_session_controller.hpp"
#include "kivo/core/render/boundary/render_boundary.hpp"
#include "kivo/core/render/pump/render_pump_result.hpp"

namespace kivo::core::playback {

class PlaybackRuntimeCoordinator final {
public:
    PlaybackRuntimeCoordinator(
        PlaybackSessionController& session,
        decode::AudioDecodeBoundary& decoder,
        render::RenderBoundary& renderer);
    ~PlaybackRuntimeCoordinator();

    PlaybackRuntimeCoordinator(const PlaybackRuntimeCoordinator&) = delete;
    PlaybackRuntimeCoordinator& operator=(const PlaybackRuntimeCoordinator&) = delete;
    PlaybackRuntimeCoordinator(PlaybackRuntimeCoordinator&&) = delete;
    PlaybackRuntimeCoordinator& operator=(PlaybackRuntimeCoordinator&&) = delete;

    [[nodiscard]] PlaybackRuntimeResult open(
        const PlaybackCommand& command,
        std::unique_ptr<decode::ByteSourceBoundary> source,
        const PlaybackRuntimeOpenRequest& request) noexcept;
    [[nodiscard]] PlaybackRuntimeResult execute(
        const PlaybackCommand& command) noexcept;
    [[nodiscard]] PlaybackRuntimeResult seek(
        const PlaybackCommand& command) noexcept;
    [[nodiscard]] PlaybackRuntimeResult recover() noexcept;

    [[nodiscard]] DecodeRenderQueueProducerResult produce_step() noexcept;
    [[nodiscard]] render::RenderPumpResult render_step() noexcept;
    [[nodiscard]] PlaybackRuntimeSnapshot snapshot() const noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace kivo::core::playback
