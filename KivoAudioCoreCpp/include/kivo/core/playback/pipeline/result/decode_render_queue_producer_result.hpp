#pragma once

#include <cstdint>

#include "kivo/core/decode/failure/decode_failure.hpp"
#include "kivo/core/render/queue/spsc_audio_block_queue.hpp"

namespace kivo::core::playback {

enum class DecodeRenderQueueProducerDisposition : uint8_t {
    Primed = 0,
    Queued,
    Backpressure,
    DiscardedStale,
    EndOfStream,
    Failed
};

struct DecodeRenderQueueProducerResult {
    DecodeRenderQueueProducerDisposition disposition{
        DecodeRenderQueueProducerDisposition::Failed};
    decode::DecodeFailure decode_failure{
        decode::DecodeFailure::None};
    render::SpscQueuePushResult queue_result{
        render::SpscQueuePushResult::Pushed};
    contract::FrameCount queued_frames{0};

    [[nodiscard]] constexpr bool operator==(
        const DecodeRenderQueueProducerResult&) const noexcept = default;
};

} // namespace kivo::core::playback
