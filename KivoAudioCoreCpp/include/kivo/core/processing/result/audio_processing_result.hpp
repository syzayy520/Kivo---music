#pragma once

#include <cstdint>

#include "kivo/core/contract/sample_position.hpp"
#include "kivo/core/processing/result/audio_processing_failure.hpp"

namespace kivo::core::processing {

enum class AudioProcessingDisposition : uint8_t {
    Bypassed = 0,
    Processed,
    Rejected,
    Failed
};

struct AudioProcessingResult {
    AudioProcessingDisposition disposition{
        AudioProcessingDisposition::Failed};
    AudioProcessingFailure failure{AudioProcessingFailure::InvalidBuffer};
    contract::FrameCount processed_frames{0};
    uint64_t clipped_samples{0};

    [[nodiscard]] bool succeeded() const noexcept {
        return disposition == AudioProcessingDisposition::Bypassed
            || disposition == AudioProcessingDisposition::Processed;
    }

    [[nodiscard]] bool operator==(
        const AudioProcessingResult&) const noexcept = default;
};

} // namespace kivo::core::processing
