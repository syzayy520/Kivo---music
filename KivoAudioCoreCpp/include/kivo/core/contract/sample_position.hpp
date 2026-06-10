// =============================================================================
// KivoAudioCoreCpp — sample_position.hpp
// Contract: Sample position and frame count type aliases
// =============================================================================
//
// Fundamental numeric types for audio position tracking.
// Platform-neutral. No runtime dependencies.
// =============================================================================

#pragma once

#include <cstdint>
#include <limits>

namespace kivo::core::contract {

// =============================================================================
// SamplePosition — Absolute sample index in a stream
// =============================================================================
using SamplePosition = uint64_t;

// =============================================================================
// FrameCount — Number of audio frames (one frame = one sample per channel)
// =============================================================================
using FrameCount = uint64_t;

// =============================================================================
// Constants
// =============================================================================
inline constexpr SamplePosition kInvalidSamplePosition = std::numeric_limits<uint64_t>::max();
inline constexpr FrameCount kZeroFrameCount = 0;

} // namespace kivo::core::contract
