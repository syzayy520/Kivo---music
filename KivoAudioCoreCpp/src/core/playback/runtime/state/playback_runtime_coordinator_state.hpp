#pragma once

#include <memory>
#include <mutex>

#include "kivo/core/playback/pipeline/decode_render_queue_producer.hpp"
#include "kivo/core/playback/runtime/coordinator/playback_runtime_coordinator.hpp"
#include "kivo/core/render/queue/spsc_audio_block_queue.hpp"

namespace kivo::core::playback {

class PlaybackRuntimeCoordinator::Impl {
public:
    Impl(
        PlaybackSessionController& session,
        decode::AudioDecodeBoundary& decoder,
        render::RenderBoundary& renderer) noexcept;
    ~Impl();

    [[nodiscard]] PlaybackRuntimeResult open(
        const PlaybackCommand& command,
        std::unique_ptr<decode::ByteSourceBoundary> source,
        const PlaybackRuntimeOpenRequest& request) noexcept;
    [[nodiscard]] PlaybackRuntimeResult execute(
        const PlaybackCommand& command) noexcept;
    [[nodiscard]] PlaybackRuntimeResult seek(
        const PlaybackCommand& command) noexcept;
    [[nodiscard]] DecodeRenderQueueProducerResult produce_step() noexcept;
    [[nodiscard]] render::RenderPumpResult render_step() noexcept;
    [[nodiscard]] PlaybackRuntimeSnapshot snapshot() const noexcept;

private:
    [[nodiscard]] bool can_open(
        const PlaybackCommand& command,
        const PlaybackSessionSnapshot& snapshot) const noexcept;
    [[nodiscard]] bool matches_active_command(
        const PlaybackCommand& command,
        const PlaybackSessionSnapshot& snapshot) const noexcept;
    [[nodiscard]] bool can_seek(
        const PlaybackCommand& command,
        const PlaybackSessionSnapshot& snapshot) const noexcept;

    [[nodiscard]] PlaybackRuntimeResult resume(
        const PlaybackCommand& command) noexcept;
    [[nodiscard]] PlaybackRuntimeResult pause(
        const PlaybackCommand& command) noexcept;
    [[nodiscard]] PlaybackRuntimeResult stop(
        const PlaybackCommand& command) noexcept;
    [[nodiscard]] PlaybackRuntimeResult flush(
        const PlaybackCommand& command) noexcept;
    [[nodiscard]] PlaybackRuntimeResult close_source(
        const PlaybackCommand& command) noexcept;
    [[nodiscard]] PlaybackRuntimeResult shutdown(
        const PlaybackCommand& command) noexcept;

    [[nodiscard]] PlaybackRuntimeResult reject_command(
        const PlaybackCommand& command) noexcept;
    [[nodiscard]] PlaybackRuntimeResult fail_current(
        uint64_t session_generation,
        PlaybackRuntimeFailure failure) noexcept;
    [[nodiscard]] PlaybackRuntimeResult fail_active(
        PlaybackRuntimeFailure failure,
        PlaybackCommandResult command = {},
        decode::DecodeFailure decode_failure = decode::DecodeFailure::None,
        render::RenderFailure render_failure =
            render::RenderFailure::None) noexcept;
    void rollback_open(
        bool replacing,
        uint64_t replaced_generation) noexcept;

    [[nodiscard]] bool release_runtime() noexcept;
    [[nodiscard]] std::unique_ptr<DecodeRenderQueueProducer>
    create_producer(
        decode::DecodeGeneration decode_generation,
        render::RenderGenerationSet generations) noexcept;
    static void saturating_increment(uint64_t& value) noexcept;

    PlaybackSessionController& session_;
    decode::AudioDecodeBoundary& decoder_;
    render::RenderBoundary& renderer_;
    mutable std::mutex mutex_{};
    std::unique_ptr<render::SpscAudioBlockQueue> queue_{};
    std::unique_ptr<DecodeRenderQueueProducer> producer_{};
    contract::RenderFormat format_{};
    render::RenderGenerationSet generations_{};
    decode::DecodeGeneration decode_generation_{};
    render::SpscAudioBlockQueueConfiguration queue_configuration_{};
    DecodeRenderQueueProducerConfiguration producer_configuration_{};
    bool active_{false};
    bool closed_{false};
    uint64_t successful_opens_{0};
    uint64_t rolled_back_opens_{0};
    uint64_t successful_seeks_{0};
    uint64_t failed_operations_{0};
    uint64_t stale_blocks_rejected_{0};
};

} // namespace kivo::core::playback
