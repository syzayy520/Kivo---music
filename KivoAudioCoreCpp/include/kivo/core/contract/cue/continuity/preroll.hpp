// =============================================================================
// KivoAudioCoreCpp — preroll.hpp
// Contract: Preroll samples for codec initialization
// =============================================================================
//
// Platform-neutral preroll reservation.
// No real decoder, no parser, no demuxer.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// Preroll — Preroll samples for codec initialization
// =============================================================================
struct Preroll {
    uint64_t samples{0};

    [[nodiscard]] bool operator==(const Preroll&) const noexcept = default;
};

} // namespace kivo::core::contract
