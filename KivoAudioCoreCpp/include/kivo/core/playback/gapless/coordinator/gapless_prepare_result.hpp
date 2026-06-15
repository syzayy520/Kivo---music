#pragma once

#include <cstdint>

#include "kivo/core/decode/failure/decode_failure.hpp"
#include "kivo/core/playback/gapless/transition/gapless_transition_decision.hpp"

namespace kivo::core::playback {

enum class GaplessPrepareFailure : uint8_t {
    None = 0,
    InvalidRequest,
    CurrentTrackFinal,
    TransitionRejected,
    SlotOccupied,
    ProducerCreationFailed,
    PrefetchFailed
};

struct GaplessPrepareResult {
    bool prepared{false};
    GaplessPrepareFailure failure{GaplessPrepareFailure::InvalidRequest};
    GaplessTransitionDecision transition{};
    decode::DecodeFailure decode_failure{decode::DecodeFailure::None};
};

} // namespace kivo::core::playback
