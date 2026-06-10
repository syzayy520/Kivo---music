// =============================================================================
// KivoAudioCoreCpp — profile.hpp
// Contract: CapabilityProfile — Identity metadata for a capability
// =============================================================================
//
// CapabilityProfile is ONLY identity metadata.
// It is NOT a capability bucket — does NOT contain HdrCapability,
// DecodeCapability, RenderCapability, OutputCapability, etc.
//
// Allowed fields: scope, subject, source, level, confidence, path, generation_id
// =============================================================================

#pragma once

#include "../../generation_id.hpp"
#include "../identity/scope.hpp"
#include "../identity/subject.hpp"
#include "../identity/source.hpp"
#include "../quality/level.hpp"
#include "../quality/confidence.hpp"
#include "../quality/path.hpp"

namespace kivo::core::contract {

// =============================================================================
// CapabilityProfile — Identity metadata only
// =============================================================================
struct CapabilityProfile {
    CapabilityScope scope = CapabilityScope::Decode;
    CapabilitySubject subject;
    CapabilitySource source;
    CapabilityLevel level = CapabilityLevel::None;
    CapabilityConfidence confidence = CapabilityConfidence::Estimated;
    CapabilityPath path = CapabilityPath::GeneralPurposePath;
    GenerationId generation_id = GenerationId::initial();

    [[nodiscard]] bool operator==(const CapabilityProfile&) const noexcept = default;
};

} // namespace kivo::core::contract
