#pragma once

#include <cstdint>

#include "kivo/core/contract/output/processing/processing_participation.hpp"
#include "kivo/core/contract/sample_position.hpp"

namespace kivo::core::processing {

struct AudioProcessingSnapshot {
    contract::ProcessingParticipation participation{};
    double replay_gain_linear{1.0};
    double volume_linear{1.0};
    double effective_gain{1.0};
    bool dither_active{false};
    bool strict_bypass{true};
    contract::FrameCount latency_frames{0};
    contract::FrameCount tail_frames{0};
    contract::FrameCount processed_frames{0};
    uint64_t processed_blocks{0};
    uint64_t clipped_samples{0};

    [[nodiscard]] bool operator==(
        const AudioProcessingSnapshot&) const noexcept = default;
};

} // namespace kivo::core::processing
