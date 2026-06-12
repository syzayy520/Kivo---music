#pragma once

#include <cstdint>

#include "kivo/core/contract/sample_position.hpp"

namespace kivo::core::playback {

enum class GaplessCoordinatorState : uint8_t {
    Ready = 0,
    Running,
    EndOfStream,
    Failed
};

struct GaplessCoordinatorSnapshot {
    GaplessCoordinatorState state{GaplessCoordinatorState::Ready};
    uint32_t current_track_index{0};
    bool next_track_prefetched{false};
    uint64_t completed_transitions{0};
    contract::FrameCount decoded_frames{0};
    contract::FrameCount queued_frames{0};
};

} // namespace kivo::core::playback
