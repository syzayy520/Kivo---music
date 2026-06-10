// =============================================================================
// KivoAudioCoreCpp — level.hpp
// Contract: CapabilityLevel — Abstract capability level
// =============================================================================
//
// Used by all capability-expressing types (HdrCapability, MemoryCapability,
// ThreadCapability, etc.) instead of bool supports_* fields.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// CapabilityLevel — Abstract capability strength
// =============================================================================
enum class CapabilityLevel : uint8_t {
    None = 0,
    Basic,
    Standard,
    High,
    Maximum
};

} // namespace kivo::core::contract
