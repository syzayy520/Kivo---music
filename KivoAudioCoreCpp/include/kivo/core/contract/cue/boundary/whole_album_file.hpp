// =============================================================================
// KivoAudioCoreCpp — whole_album_file.hpp
// Contract: Whole album file boundary definition
// =============================================================================
//
// Platform-neutral whole album file reservation.
// No file API, no real demuxer.
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

    [[nodiscard]] bool operator==(const WholeAlbumFile&) const noexcept = default;
};

} // namespace kivo::core::contract
