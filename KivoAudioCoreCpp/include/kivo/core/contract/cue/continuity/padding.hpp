// =============================================================================
// KivoAudioCoreCpp — padding.hpp
// Contract: Encoder padding in samples
// =============================================================================
//


#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// Padding — Encoder padding in samples
// =============================================================================
struct Padding {
    uint64_t samples{0};

    [[nodiscard]] constexpr bool operator==(const Padding&) const noexcept = default;
};

} // namespace kivo::core::contract
