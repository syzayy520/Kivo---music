// =============================================================================
// KivoAudioCoreCpp — seek_generation.hpp
// Contract: Seek generation tracking
// =============================================================================
//
// Tracks seek generation and count for stale frame detection.
// No real seek runtime. Platform-neutral.
// =============================================================================

#pragma once

#include "../../generation_id.hpp"
#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// SeekGeneration — Seek generation and count tracking
// =============================================================================
struct SeekGeneration {
    GenerationId generation = GenerationId::initial();
    uint32_t seek_count = 0;      // Total seeks in this session
    uint64_t last_seek_qpc = 0;   // Timestamp of last seek (platform-neutral monotonic)

    // --- Validity check ---
    [[nodiscard]] bool is_valid() const noexcept {
        // Generation must be non-initial if seeks have occurred
        if (seek_count > 0 && generation == GenerationId::initial()) return false;
        return true;
    }

    // --- Comparison ---
    [[nodiscard]] bool operator==(const SeekGeneration&) const noexcept = default;
};

} // namespace kivo::core::contract
