// =============================================================================
// KivoAudioCoreCpp — cue_sheet_boundary.hpp
// Contract: CUE sheet track boundary definition
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// CueSheetBoundary — CUE sheet track boundary definition
// =============================================================================
struct CueSheetBoundary {
    uint32_t track_index{0};
    uint64_t start_offset{0};
    uint64_t end_offset{0};
    uint64_t pregap_samples{0};

    [[nodiscard]] constexpr bool operator==(const CueSheetBoundary&) const noexcept = default;
};

} // namespace kivo::core::contract
