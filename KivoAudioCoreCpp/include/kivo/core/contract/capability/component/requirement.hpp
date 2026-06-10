// =============================================================================
// KivoAudioCoreCpp — requirement.hpp
// Contract: CapabilityRequirement — What capability is needed
// =============================================================================

#pragma once

#include "../identity/scope.hpp"
#include "../quality/level.hpp"
#include "../constraint/strictness.hpp"

namespace kivo::core::contract {

// =============================================================================
// CapabilityRequirement — Declares a capability need
// =============================================================================
struct CapabilityRequirement {
    CapabilityScope scope = CapabilityScope::Decode;
    CapabilityLevel minimum_level = CapabilityLevel::None;
    ConstraintStrictness strictness = ConstraintStrictness::Required;

    [[nodiscard]] bool operator==(const CapabilityRequirement&) const noexcept = default;
};

} // namespace kivo::core::contract
