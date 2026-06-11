// =============================================================================
// KivoAudioCoreCpp — gapless_metadata.hpp
// Contract: Gapless playback metadata
// =============================================================================
//
// Platform-neutral gapless metadata reservation.
// No real gapless playback, no P0-L implementation.
// =============================================================================

#pragma once

#include <cstdint>
#include "kivo/core/contract/cue/continuity/codec_delay.hpp"
#include "kivo/core/contract/cue/continuity/padding.hpp"
#include "kivo/core/contract/cue/continuity/preroll.hpp"
#include "kivo/core/contract/cue/continuity/prefetch.hpp"

namespace kivo::core::contract {

// =============================================================================
// GaplessMetadata — Gapless playback metadata
// =============================================================================
struct GaplessMetadata {
    CodecDelay codec_delay{};
    Padding encoder_padding{};
    Preroll preroll{};
    Prefetch prefetch{};

    [[nodiscard]] bool operator==(const GaplessMetadata&) const noexcept = default;
};

} // namespace kivo::core::contract
