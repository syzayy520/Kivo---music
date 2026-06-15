#pragma once

#include <cstddef>
#include <memory>
#include <span>

#include "kivo/core/contract/format/roles/render_format.hpp"
#include "kivo/core/contract/sample_position.hpp"
#include "kivo/core/processing/configuration/audio_processing_configuration.hpp"
#include "kivo/core/processing/result/audio_processing_plan.hpp"
#include "kivo/core/processing/result/audio_processing_result.hpp"
#include "kivo/core/processing/snapshot/audio_processing_snapshot.hpp"

namespace kivo::core::processing {

class AudioProcessingChain final {
public:
    [[nodiscard]] static std::unique_ptr<AudioProcessingChain> create(
        contract::RenderFormat format,
        AudioProcessingConfiguration configuration) noexcept;

    [[nodiscard]] AudioProcessingResult process(
        std::span<std::byte> bytes,
        contract::FrameCount frame_count) noexcept;
    [[nodiscard]] AudioProcessingSnapshot snapshot() const noexcept;

private:
    explicit AudioProcessingChain(AudioProcessingPlan plan) noexcept;

    AudioProcessingPlan plan_{};
    AudioProcessingSnapshot snapshot_{};
    uint64_t dither_state_{0};
};

} // namespace kivo::core::processing
