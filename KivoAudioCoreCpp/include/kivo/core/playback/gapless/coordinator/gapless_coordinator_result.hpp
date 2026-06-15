#pragma once

#include <cstdint>

#include "kivo/core/contract/sample_position.hpp"
#include "kivo/core/decode/failure/decode_failure.hpp"

namespace kivo::core::playback {

enum class GaplessCoordinatorDisposition : uint8_t {
    Primed = 0,
    Queued,
    Backpressure,
    TrackAdvanced,
    EndOfStream,
    Failed
};

enum class GaplessCoordinatorFailure : uint8_t {
    None = 0,
    InvalidRequest,
    TransitionRejected,
    PrefetchFailed,
    MissingPrefetchedTrack,
    ProducerFailed
};

struct GaplessCoordinatorResult {
    GaplessCoordinatorDisposition disposition{
        GaplessCoordinatorDisposition::Failed};
    GaplessCoordinatorFailure failure{
        GaplessCoordinatorFailure::InvalidRequest};
    decode::DecodeFailure decode_failure{decode::DecodeFailure::None};
    contract::FrameCount queued_frames{0};
    uint32_t current_track_index{0};
};

} // namespace kivo::core::playback
