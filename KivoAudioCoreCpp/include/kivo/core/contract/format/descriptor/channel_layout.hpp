// =============================================================================
// KivoAudioCoreCpp — channel_layout.hpp
// Contract: Speaker channel layout enumeration with metadata
// =============================================================================
//
// Platform-neutral channel layout identifier and associated constexpr metadata
// (channel count).
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// ChannelLayout — Named speaker arrangement
// =============================================================================
enum class ChannelLayout : uint8_t {
    Unknown = 0,
    Mono,       // 1.0 — single channel
    Stereo,     // 2.0 — left + right
    Surround51,  // 5.1 — front L/R, center, LFE, surround L/R
    Surround71   // 7.1 — 5.1 + surround back L/R
};

// =============================================================================
// ChannelLayout metadata — constexpr free functions
// =============================================================================

/// Number of channels for the given layout.
[[nodiscard]] constexpr uint8_t channel_layout_count(ChannelLayout layout) noexcept {
    switch (layout) {
    case ChannelLayout::Mono:        return 1;
    case ChannelLayout::Stereo:      return 2;
    case ChannelLayout::Surround51:  return 6;
    case ChannelLayout::Surround71:  return 8;
    default:                         return 0;
    }
}

/// True if the layout has more than one channel.
[[nodiscard]] constexpr bool channel_layout_is_multi_channel(ChannelLayout layout) noexcept {
    return channel_layout_count(layout) > 1;
}

} // namespace kivo::core::contract
