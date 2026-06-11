// =============================================================================
// KivoAudioCoreCpp — preroll.hpp
// Contract: Preroll samples for codec initialization
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// Preroll — Preroll samples for codec initialization
// =============================================================================
struct Preroll {
    uint64_t samples{0};

    [[nodiscard]] constexpr bool operator==(const Preroll&) const noexcept = default;
};

} // namespace kivo::core::contract
