// =============================================================================
// KivoAudioCoreCpp — seek_intent.hpp
// Contract: Seek intent (user's seek request)
// =============================================================================
//
// Defines the user's seek request with origin and offset.
// No real seek runtime. Platform-neutral.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// SeekOrigin — Reference point for seek offset
// =============================================================================
enum class SeekOrigin : uint8_t {
    Absolute,      // Absolute position from start of stream
    Relative,      // Relative to current playback position
    RelativeToEnd  // Relative to end of stream (negative offset = before end)
};

// =============================================================================
// SeekIntent — User's seek request
// =============================================================================
struct SeekIntent {
    int64_t offset = 0;           // Sample offset (interpretation depends on origin)
    SeekOrigin origin = SeekOrigin::Absolute;
    bool request_flush = true;    // Whether to flush pipeline after seek

    // --- Validity check ---
    [[nodiscard]] bool is_valid() const noexcept {
        // Absolute seek with negative offset is invalid (can't seek before start)
        if (origin == SeekOrigin::Absolute && offset < 0) return false;
        return true;
    }

    // --- Comparison ---
    [[nodiscard]] bool operator==(const SeekIntent&) const noexcept = default;
};

} // namespace kivo::core::contract
