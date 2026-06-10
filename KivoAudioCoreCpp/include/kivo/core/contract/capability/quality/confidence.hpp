// =============================================================================
// KivoAudioCoreCpp — confidence.hpp
// Contract: CapabilityConfidence — Confidence in a capability assessment
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// CapabilityConfidence — How reliable a capability assessment is
// =============================================================================
enum class CapabilityConfidence : uint8_t {
    Estimated = 0,
    Measured,
    Verified,
    Guaranteed
};

} // namespace kivo::core::contract
