#pragma once

#include <array>
#include <cstdint>

#include "kivo/core/contract/error/domain/error_domain.hpp"
#include "kivo/core/contract/format/roles/render_format.hpp"
#include "kivo/core/contract/sample_position.hpp"
#include "kivo/core/contract/output/truth/bitperfect_truth_report.hpp"
#include "kivo/core/output/truth/evidence/bit_perfect_evidence.hpp"
#include "kivo/core/decode/failure/decode_failure.hpp"
#include "kivo/core/decode/generation/decode_generation.hpp"
#include "kivo/core/observability/category/decode_failure_category.hpp"
#include "kivo/core/playback/pipeline/snapshot/decode_render_queue_producer_snapshot.hpp"
#include "kivo/core/playback/session/snapshot/playback_session_snapshot.hpp"
#include "kivo/core/render/generation/render_generation_set.hpp"
#include "kivo/core/render/queue/spsc_audio_block_queue_snapshot.hpp"
#include "kivo/core/render/snapshot/render_snapshot.hpp"

namespace kivo::core::playback {

struct PlaybackRuntimeSnapshot {
    bool active{false};
    contract::RenderFormat format{};
    // P1-3/4: real timebase for the host. total_frames is the source's full
    // length (0 / known=false when the container does not declare one).
    // source_sample_rate + resample_active expose the resample fact (truth);
    // the processed render rate lives in `format.format.sample_rate`.
    contract::FrameCount total_frames{0};
    bool total_frames_known{false};
    uint32_t source_sample_rate{0};
    bool resample_active{false};
    // P2 audio-truth: the full bit-perfect evidence + evaluated verdict, built
    // from the coordinator's captured inputs when active. Core-only C++ types;
    // the host ABI flattens them into kivo_audio_truth_v1 (never crossed raw).
    output::BitPerfectEvidence bit_perfect_evidence{};
    contract::BitPerfectTruthReport bit_perfect_report{};
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
    uint64_t format_negotiation_attempts{0};
    uint64_t format_negotiation_successes{0};
    uint64_t format_negotiation_failures{0};
    uint64_t format_renegotiation_attempts{0};
    uint64_t format_renegotiation_successes{0};
    uint64_t format_renegotiation_failures{0};
    std::array<uint64_t, observability::kDecodeFailureCategoryCount>
        decode_failures_by_category{};
    decode::DecodeFailure last_decode_failure{
        decode::DecodeFailure::None};
    contract::ErrorDomain last_decode_error_domain{
        contract::ErrorDomain::Unknown};

    [[nodiscard]] constexpr bool operator==(
        const PlaybackRuntimeSnapshot&) const noexcept = default;
};

} // namespace kivo::core::playback
