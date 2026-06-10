// =============================================================================
// KivoAudioCoreCpp — seek_flush.hpp
// Contract: Seek/Flush/Drain semantic enums (minimal)
// =============================================================================
//
// Defines seek accuracy, flush scope, and drain semantics.
// No real seek/flush runtime. Platform-neutral.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// SeekAccuracy — Precision level for seek operations
// =============================================================================
enum class SeekAccuracy : uint8_t {
    Exact,            // Seek to exact sample position
    NearestKeyframe,  // Seek to nearest keyframe (codec-dependent)
    NearestSample     // Seek to nearest sample boundary
};

// =============================================================================
// FlushScope — Bitmask for which pipeline stages to flush
// =============================================================================
enum class FlushScope : uint8_t {
    None     = 0,
    Decoder  = 1 << 0,
    Queue    = 1 << 1,
    Renderer = 1 << 2,
    All      = Decoder | Queue | Renderer
};

// --- Bitwise OR for FlushScope ---
[[nodiscard]] constexpr FlushScope operator|(FlushScope a, FlushScope b) noexcept {
    return static_cast<FlushScope>(
        static_cast<uint8_t>(a) | static_cast<uint8_t>(b)
    );
}

[[nodiscard]] constexpr FlushScope operator&(FlushScope a, FlushScope b) noexcept {
    return static_cast<FlushScope>(
        static_cast<uint8_t>(a) & static_cast<uint8_t>(b)
    );
}

[[nodiscard]] constexpr bool has_flag(FlushScope scope, FlushScope flag) noexcept {
    return (static_cast<uint8_t>(scope) & static_cast<uint8_t>(flag)) != 0;
}

// =============================================================================
// DrainSemantics — How to handle end-of-stream
// =============================================================================
enum class DrainSemantics : uint8_t {
    None,
    DrainToZero,    // Play all remaining samples
    DrainToSilence, // Play until silence threshold
    Immediate       // Stop immediately
};

} // namespace kivo::core::contract
