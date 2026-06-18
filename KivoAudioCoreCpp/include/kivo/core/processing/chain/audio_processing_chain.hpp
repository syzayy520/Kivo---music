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

    // Runtime volume update. Re-runs build_audio_processing_plan with the new
    // gain so the effective_gain / volume_active / strict_bypass / dither_active
    // tuple is recomputed by the SAME logic (and tolerance) as open time, then
    // mirrored into the snapshot. Returns false and leaves the chain unchanged
    // if the new configuration is invalid (e.g. a non-unity volume while
    // bit-perfect is required). The caller serializes this with process() and
    // snapshot() under the coordinator mutex, so the tuple is never torn.
    [[nodiscard]] bool set_volume(double volume) noexcept;

private:
    AudioProcessingChain(
        AudioProcessingPlan plan,
        AudioProcessingConfiguration configuration) noexcept;

    AudioProcessingPlan plan_{};
    AudioProcessingConfiguration configuration_{};
    AudioProcessingSnapshot snapshot_{};
    uint64_t dither_state_{0};
};

} // namespace kivo::core::processing
