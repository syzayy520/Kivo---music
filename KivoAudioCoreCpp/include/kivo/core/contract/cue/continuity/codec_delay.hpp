// =============================================================================
// KivoAudioCoreCpp — codec_delay.hpp
// Contract: Codec decoder delay in samples
// =============================================================================
//
// Platform-neutral codec delay reservation.
// No real decoder, no parser, no demuxer.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// CodecDelay — Decoder delay in samples
// =============================================================================
struct CodecDelay {
    uint64_t samples{0};

    [[nodiscard]] bool operator==(const CodecDelay&) const noexcept = default;
};

} // namespace kivo::core::contract
