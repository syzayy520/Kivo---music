// =============================================================================
// KivoAudioCoreCpp — generation_id.hpp
// Contract: Generation ID for monotonic versioning
// =============================================================================
//
// GenerationId provides a monotonically increasing identifier for tracking
// state generations across contract boundaries. Used by ClockSnapshot,
// playback state, and other contract families.
// =============================================================================

#pragma once

#include <cstdint>
#include <compare>

namespace kivo::core::contract {

// =============================================================================
// GenerationId — Monotonic version token
// =============================================================================
class GenerationId {
public:
    // --- Factory methods ---
    [[nodiscard]] static constexpr GenerationId initial() noexcept {
        return GenerationId{0};
    }

    [[nodiscard]] static constexpr GenerationId next(GenerationId current) noexcept {
        return GenerationId{current.id_ + 1};
    }

    // --- Observers ---
    [[nodiscard]] constexpr uint64_t value() const noexcept {
        return id_;
    }

    // --- Comparison (C++20 spaceship) ---
    [[nodiscard]] constexpr bool operator==(const GenerationId&) const noexcept = default;
    [[nodiscard]] constexpr auto operator<=>(const GenerationId&) const noexcept = default;

private:
    constexpr explicit GenerationId(uint64_t id) noexcept : id_(id) {}

    uint64_t id_{0};
};

} // namespace kivo::core::contract
