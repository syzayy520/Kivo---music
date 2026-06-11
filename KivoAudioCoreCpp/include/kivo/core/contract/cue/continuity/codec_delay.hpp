// =============================================================================
// KivoAudioCoreCpp — codec_delay.hpp
// Contract: Codec delay in samples
// =============================================================================
//


#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// CodecDelay — Codec delay in samples
// =============================================================================
struct CodecDelay {
    uint64_t samples{0};

    [[nodiscard]] bool operator==(const CodecDelay&) const noexcept = default;
};

} // namespace kivo::core::contract
