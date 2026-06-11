// =============================================================================
// KivoAudioCoreCpp — native_decoded_format.hpp
// Contract: Format as produced by the codec/decoder
// =============================================================================
//
// Represents the PCM format delivered by the decoding pipeline before any
// resampling or conversion. This is the "ground truth" format of the source.
// Platform-neutral.
// =============================================================================

#pragma once

#include <cstdint>

#include "../descriptor/audio_format_descriptor.hpp"

namespace kivo::core::contract {

// =============================================================================
// NativeDecodedFormat — Format after decode, before any pipeline conversions
// =============================================================================
struct NativeDecodedFormat {
    AudioFormatDescriptor format{};

    // --- Validity ---
    [[nodiscard]] bool is_valid() const noexcept {
        return format.is_valid();
    }

    // --- Comparison ---
    [[nodiscard]] constexpr bool operator==(const NativeDecodedFormat&) const noexcept = default;
};

} // namespace kivo::core::contract
