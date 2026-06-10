// =============================================================================
// KivoAudioCoreCpp — constraints.hpp
// Contract: NegotiationConstraints — Constraints for capability negotiation
// =============================================================================

#pragma once

#include "../component/requirement.hpp"
#include "../constraint/strictness.hpp"

namespace kivo::core::contract {

// =============================================================================
// NegotiationConstraints — Constraints applied during negotiation
// =============================================================================
struct NegotiationConstraints {
    CapabilityRequirement requirement;
    ConstraintStrictness strictness = ConstraintStrictness::Required;

    [[nodiscard]] bool operator==(const NegotiationConstraints&) const noexcept = default;
};

} // namespace kivo::core::contract
