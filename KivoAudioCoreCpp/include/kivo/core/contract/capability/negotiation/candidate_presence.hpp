// =============================================================================
// KivoAudioCoreCpp — candidate_presence.hpp
// Contract: CandidatePresence — Whether a negotiation candidate exists
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// CandidatePresence — Presence status of a negotiation candidate
// =============================================================================
enum class CandidatePresence : uint8_t {
    Present = 0,
    Absent,
    Unknown
};

} // namespace kivo::core::contract
