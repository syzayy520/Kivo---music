#pragma once

#include <optional>

#include "kivo/core/contract/format/descriptor/audio_format_descriptor.hpp"
#include "kivo/core/render/format/render_output_mode.hpp"

namespace kivo::core::output {

struct BitPerfectEvidence {
    std::optional<render::RenderOutputMode> requested_mode{};
    std::optional<render::RenderOutputMode> actual_mode{};
    std::optional<contract::AudioFormatDescriptor> source_format{};
    std::optional<contract::AudioFormatDescriptor> render_format{};
    std::optional<contract::AudioFormatDescriptor> device_format{};
    std::optional<bool> host_audio_engine_in_path{};
    std::optional<bool> resampler_active{};
    std::optional<bool> sample_format_conversion_active{};
    std::optional<bool> bit_depth_conversion_active{};
    std::optional<bool> channel_remix_active{};
    std::optional<bool> replay_gain_active{};
    std::optional<bool> volume_active{};
    std::optional<bool> dsp_active{};
    std::optional<bool> dither_active{};
    std::optional<bool> policy_allows_bit_perfect{};

    [[nodiscard]] constexpr bool is_complete() const noexcept {
        return requested_mode.has_value()
            && actual_mode.has_value()
            && source_format.has_value()
            && render_format.has_value()
            && device_format.has_value()
            && host_audio_engine_in_path.has_value()
            && resampler_active.has_value()
            && sample_format_conversion_active.has_value()
            && bit_depth_conversion_active.has_value()
            && channel_remix_active.has_value()
            && replay_gain_active.has_value()
            && volume_active.has_value()
            && dsp_active.has_value()
            && dither_active.has_value()
            && policy_allows_bit_perfect.has_value();
    }

    // Embedded in PlaybackRuntimeSnapshot, which uses a defaulted operator==.
    [[nodiscard]] bool operator==(
        const BitPerfectEvidence&) const noexcept = default;
};

} // namespace kivo::core::output
