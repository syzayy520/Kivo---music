// =============================================================================
// KivoAudioCoreCpp — candidate.hpp
// Contract: NegotiationCandidate — A candidate for capability negotiation
// =============================================================================
//
// NegotiationCandidate::meets_all_hard is a negotiation outcome,
// NOT a capability field.
// =============================================================================

#pragma once

#include "../component/profile.hpp"
#include "../component/decode.hpp"
#include "../component/render.hpp"
#include "../component/output.hpp"

namespace kivo::core::contract {

// =============================================================================
// NegotiationCandidate — A candidate for negotiation
// =============================================================================
struct NegotiationCandidate {
    CapabilityProfile profile;
    DecodeCapability decode;
    RenderCapability render;
    OutputCapability output;
    bool meets_all_hard = false;  // Negotiation outcome, NOT capability field

    [[nodiscard]] bool operator==(const NegotiationCandidate&) const noexcept = default;
};

} // namespace kivo::core::contract
