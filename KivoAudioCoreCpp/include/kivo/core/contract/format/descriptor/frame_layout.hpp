// =============================================================================
// KivoAudioCoreCpp — frame_layout.hpp
// Contract: Interleaved vs planar frame layout
// =============================================================================
//
// Describes how multi-channel samples are laid out in memory.
// Platform-neutral. No runtime dependencies.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// FrameLayout — Interleaved vs planar sample ordering
// =============================================================================
enum class FrameLayout : uint8_t {
    Unknown = 0,
    Interleaved,  // LRLRLR… — all channels per frame contiguous
    Planar        // LLLLL…RRRRR… — each channel contiguous
};

} // namespace kivo::core::contract
