// =============================================================================
// KivoAudioCoreCpp — result.hpp
// Contract: NegotiationResult — Outcome of capability negotiation
// =============================================================================

#pragma once

#include "candidate.hpp"
#include "candidate_presence.hpp"

namespace kivo::core::contract {

// =============================================================================
// NegotiationResult — Result of a capability negotiation
// =============================================================================
struct NegotiationResult {
    CandidatePresence presence = CandidatePresence::Unknown;
    const NegotiationCandidate* best_candidate = nullptr;  // nullptr if none
    bool all_requirements_met = false;

    [[nodiscard]] bool operator==(const NegotiationResult&) const noexcept = default;
};

} // namespace kivo::core::contract
