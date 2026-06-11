// =============================================================================
// KivoAudioCoreCpp — audio_format_descriptor.hpp
// Contract: Audio format descriptor (full)
// =============================================================================
//
// Comprehensive PCM format description combining sample format, channel layout,
// sample rate, and bit depth. Platform-neutral. No runtime dependencies.
// =============================================================================

#pragma once

#include <cstdint>

#include "sample_format.hpp"
#include "channel_layout.hpp"

namespace kivo::core::contract {

// =============================================================================
// AudioFormatDescriptor — PCM format metadata
// =============================================================================
struct AudioFormatDescriptor {
    SampleFormat sample_format{SampleFormat::Unknown};
    ChannelLayout channel_layout{ChannelLayout::Unknown};
    uint32_t sample_rate{0};
    uint8_t bits_per_sample{0};

    // --- Channel count (derived from layout) ---
    [[nodiscard]] constexpr uint8_t channel_count() const noexcept {
        return channel_layout_count(channel_layout);
    }

    // --- Frame size in bytes ---
    [[nodiscard]] constexpr uint8_t bytes_per_frame() const noexcept {
        return (bits_per_sample / 8u) * channel_count();
    }

    // --- Validity check ---
    [[nodiscard]] constexpr bool is_valid() const noexcept {
        if (sample_format == SampleFormat::Unknown) return false;
        if (channel_layout == ChannelLayout::Unknown) return false;
        if (sample_rate == 0) return false;
        if (bits_per_sample == 0) return false;
        return true;
    }

    // --- Comparison ---
    [[nodiscard]] constexpr bool operator==(const AudioFormatDescriptor&) const noexcept = default;
};

} // namespace kivo::core::contract
