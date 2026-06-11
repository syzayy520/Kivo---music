// =============================================================================
// KivoAudioCoreCpp — realtime_path.hpp
// Contract: Whether the current execution path is realtime
// =============================================================================
//
// Expresses whether the current execution path is in the realtime domain.
// Contract state only. No runtime thread detection.
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// RealtimePath — Realtime path contract state
// =============================================================================
enum class RealtimePath : uint8_t {
    Unknown = 0,
    Realtime,
    NonRealtime
};

} // namespace kivo::core::contract