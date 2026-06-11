// =============================================================================
// KivoAudioCoreCpp — conversion_policy.hpp
// Contract: What format conversions are required or permitted
// =============================================================================
//
// Describes the set of conversions needed between pipeline stages.
// Platform-neutral. No codec/platform-specific conversion logic.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// ConversionPolicy — Required format conversions
// =============================================================================
struct ConversionPolicy {
    bool sample_format_convert{false};    // change sample encoding
    bool sample_rate_convert{false};      // resample to different rate
    bool channel_remix{false};            // remix channel layout
    bool bit_depth_convert{false};        // change bit depth

    // --- Convenience ---
    [[nodiscard]] bool any_conversion_needed() const noexcept {
        return sample_format_convert
            || sample_rate_convert
            || channel_remix
            || bit_depth_convert;
    }

    [[nodiscard]] bool is_passthrough() const noexcept {
        return !any_conversion_needed();
    }

    // --- Comparison ---
    [[nodiscard]] bool operator==(const ConversionPolicy&) const noexcept = default;
};

} // namespace kivo::core::contract
