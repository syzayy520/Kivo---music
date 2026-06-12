#pragma once

#include <array>
#include <memory>
#include <mutex>

#include "kivo/core/playback/pipeline/producer/decode_render_queue_producer.hpp"
#include "kivo/core/playback/runtime/coordinator/playback_runtime_coordinator.hpp"
#include "kivo/core/observability/category/decode_failure_category.hpp"
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
    [[nodiscard]] PlaybackRuntimeResult recover() noexcept;
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
    [[nodiscard]] render::RenderPumpResult complete_end_of_stream(
        render::RenderPumpResult result,
        uint64_t session_generation) noexcept;
    [[nodiscard]] bool request_device_recovery(
        uint64_t session_generation) noexcept;
    void handle_decode_failure(
        decode::DecodeFailure failure,
        uint64_t session_generation) noexcept;
    void record_decode_failure(
        decode::DecodeFailure failure) noexcept;
    void record_format_negotiation_attempt(
        bool renegotiation) noexcept;
    void record_format_negotiation_success(
        bool renegotiation) noexcept;
    void record_format_negotiation_failure(
        bool renegotiation) noexcept;

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
        uint64_t replaced_generation,
        contract::ErrorDomain domain =
            contract::ErrorDomain::Unknown) noexcept;

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
    render::RenderOpenRequest render_open_request_{};
    render::RenderDrainRequest drain_request_{
        std::chrono::seconds{2}};
    bool active_{false};
    bool closed_{false};
    bool device_recovery_pending_{false};
    uint64_t successful_opens_{0};
    uint64_t rolled_back_opens_{0};
    uint64_t successful_seeks_{0};
    uint64_t failed_operations_{0};
    uint64_t stale_blocks_rejected_{0};
    uint64_t drain_attempts_{0};
    uint64_t successful_drains_{0};
    uint64_t failed_drains_{0};
    uint64_t drain_timeouts_{0};
    uint64_t device_loss_events_{0};
    uint64_t device_recovery_attempts_{0};
    uint64_t successful_device_recoveries_{0};
    uint64_t failed_device_recoveries_{0};
    uint64_t decode_failure_events_{0};
    uint64_t recoverable_decode_failures_{0};
    uint64_t decode_fallback_stops_{0};
    uint64_t failed_decode_recoveries_{0};
    uint64_t format_negotiation_attempts_{0};
    uint64_t format_negotiation_successes_{0};
    uint64_t format_negotiation_failures_{0};
    uint64_t format_renegotiation_attempts_{0};
    uint64_t format_renegotiation_successes_{0};
    uint64_t format_renegotiation_failures_{0};
    std::array<uint64_t, observability::kDecodeFailureCategoryCount>
        decode_failures_by_category_{};
    decode::DecodeFailure last_decode_failure_{
        decode::DecodeFailure::None};
    contract::ErrorDomain last_decode_error_domain_{
        contract::ErrorDomain::Unknown};
};

} // namespace kivo::core::playback
