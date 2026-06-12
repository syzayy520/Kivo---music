#pragma once

#include <cstdint>

#include "kivo/core/contract/sample_position.hpp"
#include "kivo/core/decode/failure/decode_failure.hpp"
#include "kivo/core/processing/snapshot/audio_processing_snapshot.hpp"

namespace kivo::core::playback {

enum class DecodeRenderQueueProducerState : uint8_t {
    Ready = 0,
    Running,
    PrefetchedEndOfStream,
    EndOfStream,
    Failed
};

struct DecodeRenderQueueProducerSnapshot {
    DecodeRenderQueueProducerState state{
        DecodeRenderQueueProducerState::Ready};
    decode::DecodeFailure last_decode_failure{
        decode::DecodeFailure::None};
    contract::FrameCount decoded_frames{0};
    contract::FrameCount queued_frames{0};
    uint64_t decoded_blocks{0};
    uint64_t queued_blocks{0};
    uint64_t backpressure_count{0};
    uint64_t stale_blocks_discarded{0};
    processing::AudioProcessingSnapshot processing{};

    [[nodiscard]] bool operator==(
        const DecodeRenderQueueProducerSnapshot&) const noexcept = default;
};

} // namespace kivo::core::playback
