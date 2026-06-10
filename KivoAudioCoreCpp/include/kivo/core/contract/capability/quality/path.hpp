// =============================================================================
// KivoAudioCoreCpp — path.hpp
// Contract: CapabilityPath — Processing path type
// =============================================================================
//
// DedicatedPath:    Optimized single-purpose path
// GeneralPurposePath: Flexible multi-use path
// ExternalPath:     Delegated to external system
//
// FORBIDDEN values: HardwareDecode, SoftwareDecode,
//                   HardwareAcceleratedPath, EmulatedPath
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// CapabilityPath — Processing path classification
// =============================================================================
enum class CapabilityPath : uint8_t {
    DedicatedPath = 0,
    GeneralPurposePath,
    ExternalPath
};

} // namespace kivo::core::contract
