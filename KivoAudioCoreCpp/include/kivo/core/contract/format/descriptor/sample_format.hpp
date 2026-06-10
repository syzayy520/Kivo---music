// =============================================================================
// KivoAudioCoreCpp — sample_format.hpp
// Contract: PCM sample format enumeration with metadata
// =============================================================================
//
// Platform-neutral sample format identifier and associated constexpr metadata
// (valid bits, container bits, signedness, float).
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// SampleFormat — PCM sample encoding identifier
// =============================================================================
enum class SampleFormat : uint8_t {
    Unknown = 0,
    Int16,    // Signed 16-bit integer
    Int24,    // Signed 24-bit integer (in 32-bit container)
    Int32,    // Signed 32-bit integer
    Float32,  // IEEE 754 single-precision float
    Float64   // IEEE 754 double-precision float
};

// =============================================================================
// SampleFormat metadata — constexpr free functions
// =============================================================================

/// Number of bits that carry actual signal data.
[[nodiscard]] constexpr uint8_t sample_format_valid_bits(SampleFormat fmt) noexcept {
    switch (fmt) {
    case SampleFormat::Int16:   return 16;
    case SampleFormat::Int24:   return 24;
    case SampleFormat::Int32:   return 32;
    case SampleFormat::Float32: return 32;
    case SampleFormat::Float64: return 64;
    default:                    return 0;
    }
}

/// Number of bits in the memory container (may be larger than valid bits).
[[nodiscard]] constexpr uint8_t sample_format_container_bits(SampleFormat fmt) noexcept {
    switch (fmt) {
    case SampleFormat::Int16:   return 16;
    case SampleFormat::Int24:   return 32;
    case SampleFormat::Int32:   return 32;
    case SampleFormat::Float32: return 32;
    case SampleFormat::Float64: return 64;
    default:                    return 0;
    }
}

/// True if the sample encoding is integer (signed or unsigned).
[[nodiscard]] constexpr bool sample_format_is_integer(SampleFormat fmt) noexcept {
    return fmt == SampleFormat::Int16
        || fmt == SampleFormat::Int24
        || fmt == SampleFormat::Int32;
}

/// True if the sample encoding is signed integer.
[[nodiscard]] constexpr bool sample_format_is_signed(SampleFormat fmt) noexcept {
    // All integer formats in this contract are signed.
    return sample_format_is_integer(fmt);
}

/// True if the sample encoding is floating-point.
[[nodiscard]] constexpr bool sample_format_is_float(SampleFormat fmt) noexcept {
    return fmt == SampleFormat::Float32
        || fmt == SampleFormat::Float64;
}

/// Natural alignment of a single sample in bytes.
[[nodiscard]] constexpr uint8_t sample_format_alignment(SampleFormat fmt) noexcept {
    return (sample_format_container_bits(fmt) + 7u) / 8u;
}

} // namespace kivo::core::contract
