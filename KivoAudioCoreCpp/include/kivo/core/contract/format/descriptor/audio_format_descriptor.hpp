// =============================================================================
// KivoAudioCoreCpp — audio_format_descriptor.hpp
// Contract: Audio format descriptor (full)
// =============================================================================
//
// Comprehensive PCM format description combining sample format, channel layout,
// sample rate, valid bit depth, and container bit depth. Platform-neutral.
// No runtime dependencies.
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
    uint8_t container_bits_per_sample{0};

    // --- Channel count (derived from layout) ---
    [[nodiscard]] constexpr uint8_t channel_count() const noexcept {
        return channel_layout_count(channel_layout);
    }

    // --- Valid signal bits ---
    [[nodiscard]] constexpr uint8_t valid_bits_per_sample() const noexcept {
        return bits_per_sample;
    }

    // --- Memory container bits ---
    [[nodiscard]] constexpr uint8_t container_bits() const noexcept {
        return container_bits_per_sample == 0
            ? sample_format_container_bits(sample_format)
            : container_bits_per_sample;
    }

    // --- Frame size in bytes ---
    [[nodiscard]] constexpr uint8_t bytes_per_frame() const noexcept {
        return static_cast<uint8_t>((container_bits() / 8u) * channel_count());
    }

    // --- Validity check ---
    [[nodiscard]] constexpr bool is_valid() const noexcept {
        if (sample_format == SampleFormat::Unknown) return false;
        if (channel_layout == ChannelLayout::Unknown) return false;
        if (sample_rate == 0) return false;
        if (bits_per_sample == 0) return false;

        const auto expected_valid_bits = sample_format_valid_bits(sample_format);
        const auto expected_container_bits = sample_format_container_bits(sample_format);
        const auto actual_container_bits = container_bits();

        if (expected_valid_bits == 0) return false;
        if (expected_container_bits == 0) return false;
        if (bits_per_sample != expected_valid_bits) return false;
        if (actual_container_bits != expected_container_bits) return false;
        if (bits_per_sample > actual_container_bits) return false;
        if ((actual_container_bits % 8u) != 0) return false;

        return true;
    }

    // --- Comparison ---
    [[nodiscard]] constexpr bool operator==(const AudioFormatDescriptor& other) const noexcept {
        return sample_format == other.sample_format
            && channel_layout == other.channel_layout
            && sample_rate == other.sample_rate
            && bits_per_sample == other.bits_per_sample
            && container_bits() == other.container_bits();
    }
};

} // namespace kivo::core::contract
