#pragma once

#include <cstdint>

#include "kivo/core/contract/format/roles/render_format.hpp"
#include "kivo/core/decode/generation/decode_generation.hpp"
#include "kivo/core/playback/pipeline/decode_render_queue_producer_snapshot.hpp"
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

    [[nodiscard]] constexpr bool operator==(
        const PlaybackRuntimeSnapshot&) const noexcept = default;
};

} // namespace kivo::core::playback
