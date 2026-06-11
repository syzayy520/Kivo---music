// =============================================================================
// KivoAudioCoreCpp — prefetch.hpp
// Contract: Prefetch samples for seamless playback
// =============================================================================
//
// Platform-neutral prefetch reservation.
// No real playback, no buffer management.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// Prefetch — Prefetch samples for seamless playback
// =============================================================================
struct Prefetch {
    uint64_t samples{0};

    [[nodiscard]] bool operator==(const Prefetch&) const noexcept = default;
};

} // namespace kivo::core::contract
