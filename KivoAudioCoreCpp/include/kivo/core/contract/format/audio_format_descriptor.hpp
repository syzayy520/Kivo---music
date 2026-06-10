// =============================================================================
// KivoAudioCoreCpp — audio_format_descriptor.hpp
// Contract: Audio format descriptor (minimal)
// =============================================================================
//
// Describes the PCM format of audio data. No resampling, no channel remix.
// Platform-neutral. No runtime dependencies.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// SampleFormat — PCM sample encoding
// =============================================================================
enum class SampleFormat : uint8_t {
    Unknown = 0,
    Int16,
    Int24,
    Int32,
    Float32,
    Float64
};

// =============================================================================
// ChannelLayout — Speaker arrangement
// =============================================================================
enum class ChannelLayout : uint8_t {
    Unknown = 0,
    Mono,
    Stereo,
    Surround51,
    Surround71
};

// =============================================================================
// AudioFormatDescriptor — PCM format metadata
// =============================================================================
struct AudioFormatDescriptor {
    SampleFormat sample_format = SampleFormat::Unknown;
    ChannelLayout channel_layout = ChannelLayout::Unknown;
    uint32_t sample_rate = 0;
    uint8_t bits_per_sample = 0;

    // --- Validity check ---
    [[nodiscard]] bool is_valid() const noexcept {
        if (sample_format == SampleFormat::Unknown) return false;
        if (channel_layout == ChannelLayout::Unknown) return false;
        if (sample_rate == 0) return false;
        if (bits_per_sample == 0) return false;
        return true;
    }

    // --- Comparison ---
    [[nodiscard]] bool operator==(const AudioFormatDescriptor&) const noexcept = default;
};

} // namespace kivo::core::contract
