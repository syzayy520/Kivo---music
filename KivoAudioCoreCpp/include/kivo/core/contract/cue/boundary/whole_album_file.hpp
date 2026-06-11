// =============================================================================
// KivoAudioCoreCpp — whole_album_file.hpp
// Contract: Whole album file boundary definition
// =============================================================================


#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// WholeAlbumFile — Whole album file boundary definition
// =============================================================================
struct WholeAlbumFile {
    uint64_t total_samples{0};
    uint32_t track_count{0};
    bool is_single_file{false};

    [[nodiscard]] constexpr bool operator==(const WholeAlbumFile&) const noexcept = default;
};

} // namespace kivo::core::contract
