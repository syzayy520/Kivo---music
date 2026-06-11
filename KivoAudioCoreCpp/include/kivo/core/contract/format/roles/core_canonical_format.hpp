// =============================================================================
// KivoAudioCoreCpp — core_canonical_format.hpp
// Contract: Core engine internal working format
// =============================================================================
//
// Represents the format used internally by the audio core for all processing
// (mixing, DSP, volume). All sources are converted to this format before
// further pipeline stages. Platform-neutral.
// =============================================================================

#pragma once

#include <cstdint>

#include "../descriptor/audio_format_descriptor.hpp"

namespace kivo::core::contract {

// =============================================================================
// CoreCanonicalFormat — Core's internal processing format
// =============================================================================
struct CoreCanonicalFormat {
    AudioFormatDescriptor format{};

    // --- Validity ---
    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return format.is_valid();
    }

    // --- Comparison ---
    [[nodiscard]] constexpr bool operator==(const CoreCanonicalFormat&) const noexcept = default;
};

} // namespace kivo::core::contract
