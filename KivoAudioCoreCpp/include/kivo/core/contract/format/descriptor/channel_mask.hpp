// =============================================================================
// KivoAudioCoreCpp — channel_mask.hpp
// Contract: Platform-neutral channel mask equivalent
// =============================================================================
//
// Represents a platform-neutral bitmask of speaker positions.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// ChannelMaskEquivalent — Platform-neutral speaker position bitmask
// =============================================================================
//
// Each bit position corresponds to a logical speaker role.
// The interpretation is platform-neutral.
// =============================================================================
enum class ChannelMaskBit : uint32_t {
    None            = 0x00000000,
    FrontLeft       = 0x00000001,
    FrontRight      = 0x00000002,
    FrontCenter     = 0x00000004,
    LowFrequency    = 0x00000008,
    BackLeft        = 0x00000010,
    BackRight       = 0x00000020,
    FrontLeftCenter  = 0x00000040,
    FrontRightCenter = 0x00000080,
    BackCenter      = 0x00000100,
    SideLeft        = 0x00000200,
    SideRight       = 0x00000400,
    TopCenter       = 0x00000800,
    TopFrontLeft    = 0x00001000,
    TopFrontCenter  = 0x00002000,
    TopFrontRight   = 0x00004000,
    TopBackLeft     = 0x00008000,
    TopBackCenter   = 0x00010000,
    TopBackRight    = 0x00020000
};

// =============================================================================
// ChannelMask — Aggregated bitmask of speaker positions
// =============================================================================
struct ChannelMask {
    uint32_t value = 0;

    // --- Helpers ---
    [[nodiscard]] constexpr bool has(ChannelMaskBit bit) const noexcept {
        return (value & static_cast<uint32_t>(bit)) != 0;
    }

    [[nodiscard]] constexpr bool is_stereo() const noexcept {
        return has(ChannelMaskBit::FrontLeft)
            && has(ChannelMaskBit::FrontRight)
            && (value & ~0x00000003u) == 0;
    }

    // --- Comparison ---
    [[nodiscard]] bool operator==(const ChannelMask&) const noexcept = default;
};

// =============================================================================
// Predefined channel masks for common layouts
// =============================================================================
inline constexpr ChannelMask kMonoMask = {
    static_cast<uint32_t>(ChannelMaskBit::FrontCenter)
};

inline constexpr ChannelMask kStereoMask = {
    static_cast<uint32_t>(ChannelMaskBit::FrontLeft)
    | static_cast<uint32_t>(ChannelMaskBit::FrontRight)
};

inline constexpr ChannelMask kSurround51Mask = {
    static_cast<uint32_t>(ChannelMaskBit::FrontLeft)
    | static_cast<uint32_t>(ChannelMaskBit::FrontRight)
    | static_cast<uint32_t>(ChannelMaskBit::FrontCenter)
    | static_cast<uint32_t>(ChannelMaskBit::LowFrequency)
    | static_cast<uint32_t>(ChannelMaskBit::BackLeft)
    | static_cast<uint32_t>(ChannelMaskBit::BackRight)
};

inline constexpr ChannelMask kSurround71Mask = {
    static_cast<uint32_t>(ChannelMaskBit::FrontLeft)
    | static_cast<uint32_t>(ChannelMaskBit::FrontRight)
    | static_cast<uint32_t>(ChannelMaskBit::FrontCenter)
    | static_cast<uint32_t>(ChannelMaskBit::LowFrequency)
    | static_cast<uint32_t>(ChannelMaskBit::BackLeft)
    | static_cast<uint32_t>(ChannelMaskBit::BackRight)
    | static_cast<uint32_t>(ChannelMaskBit::SideLeft)
    | static_cast<uint32_t>(ChannelMaskBit::SideRight)
};

} // namespace kivo::core::contract
