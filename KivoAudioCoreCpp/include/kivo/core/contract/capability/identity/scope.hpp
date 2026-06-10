// =============================================================================
// KivoAudioCoreCpp — scope.hpp
// Contract: CapabilityScope — Scope of a capability assessment
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// CapabilityScope — What domain a capability belongs to
// =============================================================================
enum class CapabilityScope : uint8_t {
    Decode = 0,
    Render,
    Convert,
    Output,
    Source,
    Clock,
    Memory,
    Thread
};

} // namespace kivo::core::contract
