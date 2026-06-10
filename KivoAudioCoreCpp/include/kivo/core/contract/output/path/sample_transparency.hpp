// =============================================================================
// KivoAudioCoreCpp — sample_transparency.hpp
// Contract: Whether the sample path is transparent
// =============================================================================
//
// Expresses SampleTransparentPath truth — no sample format conversion
// in the output path. Platform-neutral. No runtime sample comparison.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// SampleTransparency — Sample path transparency confirmation
// =============================================================================
struct SampleTransparency {
    bool sample_path_transparent = false;
    bool bit_depth_preserved = false;
    bool encoding_preserved = false;

    // --- Convenience ---
    [[nodiscard]] bool is_transparent() const noexcept {
        return sample_path_transparent;
    }

    // --- Comparison ---
    [[nodiscard]] bool operator==(const SampleTransparency&) const noexcept = default;
};

} // namespace kivo::core::contract
