#pragma once

#include "kivo/core/contract/format/roles/render_format.hpp"
#include "kivo/core/contract/output/processing/processing_participation.hpp"
#include "kivo/core/processing/configuration/audio_processing_configuration.hpp"
#include "kivo/core/processing/result/audio_processing_failure.hpp"

namespace kivo::core::processing {

struct AudioProcessingPlan {
    contract::RenderFormat format{};
    contract::ProcessingParticipation participation{};
    AudioProcessingFailure failure{AudioProcessingFailure::None};
    double replay_gain_linear{1.0};
    double volume_linear{1.0};
    double effective_gain{1.0};
    bool dither_active{false};
    bool strict_bypass{true};
    uint64_t dither_seed{0};

    [[nodiscard]] bool is_valid() const noexcept {
        return format.is_valid()
            && failure == AudioProcessingFailure::None
            && dither_seed != 0;
    }
};

[[nodiscard]] AudioProcessingPlan build_audio_processing_plan(
    contract::RenderFormat format,
    const AudioProcessingConfiguration& configuration) noexcept;

} // namespace kivo::core::processing
