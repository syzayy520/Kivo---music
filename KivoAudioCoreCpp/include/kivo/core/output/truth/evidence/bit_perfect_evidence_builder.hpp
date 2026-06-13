#pragma once

#include "kivo/core/output/truth/evidence/bit_perfect_evidence.hpp"
#include "kivo/core/processing/snapshot/audio_conversion_snapshot.hpp"
#include "kivo/core/processing/snapshot/audio_processing_snapshot.hpp"

namespace kivo::core::output {

[[nodiscard]] BitPerfectEvidence build_bit_perfect_evidence(
    render::RenderOutputMode requested_mode,
    render::RenderOutputMode actual_mode,
    const contract::AudioFormatDescriptor& source_format,
    const contract::AudioFormatDescriptor& render_format,
    const contract::AudioFormatDescriptor& device_format,
    bool host_audio_engine_in_path,
    bool policy_allows_bit_perfect,
    const processing::AudioConversionSnapshot& conversion,
    const processing::AudioProcessingSnapshot& processing) noexcept;

} // namespace kivo::core::output
