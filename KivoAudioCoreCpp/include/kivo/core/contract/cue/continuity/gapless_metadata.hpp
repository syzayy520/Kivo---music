// =============================================================================
// KivoAudioCoreCpp — gapless_metadata.hpp
// Contract: Gapless media metadata
// =============================================================================
//


#pragma once

#include <cstdint>
#include "kivo/core/contract/cue/continuity/codec_delay.hpp"
#include "kivo/core/contract/cue/continuity/padding.hpp"
#include "kivo/core/contract/cue/continuity/preroll.hpp"
#include "kivo/core/contract/cue/continuity/prefetch.hpp"

namespace kivo::core::contract {

// =============================================================================
// GaplessMetadata — Gapless media metadata
// =============================================================================
struct GaplessMetadata {
    CodecDelay codec_delay{};
    Padding encoder_padding{};
    Preroll preroll{};
    Prefetch prefetch{};

    [[nodiscard]] constexpr bool operator==(const GaplessMetadata&) const noexcept = default;
};

} // namespace kivo::core::contract
