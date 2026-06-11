// =============================================================================
// KivoAudioCoreCpp — prefetch.hpp
// Contract: Prefetch samples for seamless media
// =============================================================================
//


#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// Prefetch — Prefetch samples for seamless media
// =============================================================================
struct Prefetch {
    uint64_t samples{0};

    [[nodiscard]] bool operator==(const Prefetch&) const noexcept = default;
};

} // namespace kivo::core::contract
