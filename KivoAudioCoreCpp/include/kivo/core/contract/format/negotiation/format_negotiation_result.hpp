// =============================================================================
// KivoAudioCoreCpp — format_negotiation_result.hpp
// Contract: Aggregate outcome of format negotiation
// =============================================================================
//
// Summarizes the complete format negotiation chain:
// NativeDecodedFormat → CoreCanonicalFormat → RenderFormat → DeviceFormat.
// Platform-neutral. No runtime negotiation logic.
// =============================================================================

#pragma once

#include <cstdint>

#include "../descriptor/audio_format_descriptor.hpp"

namespace kivo::core::contract {

// =============================================================================
// FormatNegotiationResult — Full negotiation outcome
// =============================================================================
struct FormatNegotiationResult {
    AudioFormatDescriptor native_format;       // source format from decoder
    AudioFormatDescriptor core_format;          // internal processing format
    AudioFormatDescriptor render_format;        // format at render boundary
    AudioFormatDescriptor device_format;        // format on device output
    bool resample_needed{false};
    bool conversion_needed{false};
    bool bit_perfect_possible{false};

    // --- Convenience ---
    [[nodiscard]] bool formats_match() const noexcept {
        return native_format == render_format
            && native_format == device_format;
    }

    // --- Comparison ---
    [[nodiscard]] constexpr bool operator==(const FormatNegotiationResult&) const noexcept = default;
};

} // namespace kivo::core::contract
