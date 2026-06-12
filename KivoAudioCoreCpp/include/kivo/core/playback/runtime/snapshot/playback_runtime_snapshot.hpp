#pragma once

#include <cstdint>

#include "kivo/core/contract/error/domain/error_domain.hpp"
#include "kivo/core/contract/format/roles/render_format.hpp"
#include "kivo/core/decode/failure/decode_failure.hpp"
#include "kivo/core/decode/generation/decode_generation.hpp"
#include "kivo/core/playback/pipeline/snapshot/decode_render_queue_producer_snapshot.hpp"
#include "kivo/core/playback/session/snapshot/playback_session_snapshot.hpp"
#include "kivo/core/render/generation/render_generation_set.hpp"
#include "kivo/core/render/queue/spsc_audio_block_queue_snapshot.hpp"
#include "kivo/core/render/snapshot/render_snapshot.hpp"

namespace kivo::core::playback {

struct PlaybackRuntimeSnapshot {
    bool active{false};
    contract::RenderFormat format{};
    decode::DecodeGeneration decode_generation{};
    render::RenderGenerationSet render_generations{};
    PlaybackSessionSnapshot session{};
    render::RenderSnapshot renderer{};
    render::SpscAudioBlockQueueSnapshot queue{};
    DecodeRenderQueueProducerSnapshot producer{};
    uint64_t successful_opens{0};
    uint64_t rolled_back_opens{0};
    uint64_t successful_seeks{0};
    uint64_t failed_operations{0};
    uint64_t stale_blocks_rejected{0};
    uint64_t drain_attempts{0};
    uint64_t successful_drains{0};
    uint64_t failed_drains{0};
    uint64_t drain_timeouts{0};
    bool device_recovery_pending{false};
    uint64_t device_loss_events{0};
    uint64_t device_recovery_attempts{0};
    uint64_t successful_device_recoveries{0};
    uint64_t failed_device_recoveries{0};
    uint64_t decode_failure_events{0};
    uint64_t recoverable_decode_failures{0};
    uint64_t decode_fallback_stops{0};
    uint64_t failed_decode_recoveries{0};
    decode::DecodeFailure last_decode_failure{
        decode::DecodeFailure::None};
    contract::ErrorDomain last_decode_error_domain{
        contract::ErrorDomain::Unknown};

    [[nodiscard]] constexpr bool operator==(
        const PlaybackRuntimeSnapshot&) const noexcept = default;
};

} // namespace kivo::core::playback
